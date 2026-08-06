/*
 * yolov8npose_decode.c
 *
 * Siehe yolov8npose_decode.h fuer Layout und Schnittstelle.
 */

#include <stdio.h>
#include <stddef.h>

#include "yolov8npose_decode.h"

#define ROWS      YOLOV8NPOSE_OUTPUT_ROWS      /* 56   */
#define ANCHORS   YOLOV8NPOSE_OUTPUT_ANCHORS   /* 2100 */
#define NJOINTS   YOLOV8NPOSE_NUM_JOINTS       /* 17   */

#define ROW_SCORE      4   /* Zeile des Personen-Scores        */
#define ROW_JOINTS     5   /* erste Gelenkzeile, dann je 3     */

/* Groesse der Kandidatenliste vor der Ueberlappungsfilterung. Mehr als 32
 * Anker ueber der Schwelle treten selbst bei dicht besetzten Bildern nicht
 * auf, und die schwaechsten waeren ohnehin die ersten Opfer der Filterung. */
#define MAX_CANDIDATES 32

/* Zeiger auf den Anfang einer Tensorzeile. */
#define ROW_PTR(out, r)  ((out) + (size_t)(r) * (size_t)ANCHORS)

const char *const yolov8npose_joint_names[NJOINTS] = {
    "nose",           "left_eye",       "right_eye",
    "left_ear",       "right_ear",      "left_shoulder",
    "right_shoulder", "left_elbow",     "right_elbow",
    "left_wrist",     "right_wrist",    "left_hip",
    "right_hip",      "left_knee",      "right_knee",
    "left_ankle",     "right_ankle"
};

typedef struct {
    int    anchor;
    int8_t q;      /* quantisierter Personen-Score */
} candidate_t;

/* Skelett in COCO-Konvention: 19 Kanten ueber die 17 Gelenkindizes. */
#define NUM_EDGES 19

static const uint8_t skeleton[NUM_EDGES][2] = {
    { 15, 13 }, { 13, 11 }, { 16, 14 }, { 14, 12 }, { 11, 12 },  /* Beine, Becken */
    {  5, 11 }, {  6, 12 }, {  5,  6 },                          /* Rumpf         */
    {  5,  7 }, {  7,  9 }, {  6,  8 }, {  8, 10 },              /* Arme          */
    {  0,  1 }, {  0,  2 }, {  1,  3 }, {  2,  4 },              /* Kopf          */
    {  1,  2 }, {  3,  5 }, {  4,  6 }                           /* Hals          */
};

/* Dequantisierung des Ausgangstensors, ein Zahlenpaar fuer alle Zeilen. */
static inline float dequant(int8_t q)
{
    return ((float) q - (float) YOLOV8NPOSE_OUTPUT_ZERO_POINT)
            * YOLOV8NPOSE_OUTPUT_SCALE;
}

/* Kleinste int8-Zahl, deren dequantisierter Wert die Schwelle erreicht.
 * Damit laeuft die Vorauswahl ueber alle 2100 Anker rein ganzzahlig. */
static inline int quant_threshold(float score)
{
    int q = (int) (score / YOLOV8NPOSE_OUTPUT_SCALE + 0.5f)
            + YOLOV8NPOSE_OUTPUT_ZERO_POINT;

    if (q < -128) { q = -128; }
    if (q >  127) { q =  127; }
    return q;
}

/* Kandidat einsortieren, absteigend nach Score. Ist die Liste voll, faellt
 * der schwaechste Eintrag heraus. Rueckgabe: neue Laenge der Liste. */
static int insert_candidate(candidate_t *list, int len, int anchor, int8_t q)
{
    int pos = len;

    if (len == MAX_CANDIDATES)
    {
        if (q <= list[len - 1].q)
        {
            return len;
        }
        pos = len - 1;   /* schwaechsten Eintrag verdraengen */
    }
    else
    {
        len++;
    }

    while (pos > 0 && list[pos - 1].q < q)
    {
        list[pos] = list[pos - 1];
        pos--;
    }

    list[pos].anchor = anchor;
    list[pos].q      = q;

    return len;
}

/* Box eines Ankers aus cx, cy, w, h in Eckpunkte der Modellflaeche. */
static void decode_box(const int8_t *out, int anchor, yolov8npose_person_t *p)
{
    const float side = (float) YOLOV8NPOSE_INPUT_WIDTH;

    const float cx = dequant(ROW_PTR(out, 0)[anchor]) * side;
    const float cy = dequant(ROW_PTR(out, 1)[anchor]) * side;
    const float w  = dequant(ROW_PTR(out, 2)[anchor]) * side;
    const float h  = dequant(ROW_PTR(out, 3)[anchor]) * side;

    p->x1 = cx - 0.5f * w;
    p->y1 = cy - 0.5f * h;
    p->x2 = cx + 0.5f * w;
    p->y2 = cy + 0.5f * h;
}

static void decode_joints(const int8_t *out, int anchor, yolov8npose_person_t *p)
{
    const float side = (float) YOLOV8NPOSE_INPUT_WIDTH;

    for (int k = 0; k < NJOINTS; k++)
    {
        const int row = ROW_JOINTS + 3 * k;

        p->joints[k].x     = dequant(ROW_PTR(out, row + 0)[anchor]) * side;
        p->joints[k].y     = dequant(ROW_PTR(out, row + 1)[anchor]) * side;
        p->joints[k].score = dequant(ROW_PTR(out, row + 2)[anchor]);
    }
}

static float iou(const yolov8npose_person_t *a, const yolov8npose_person_t *b)
{
    const float x1 = a->x1 > b->x1 ? a->x1 : b->x1;
    const float y1 = a->y1 > b->y1 ? a->y1 : b->y1;
    const float x2 = a->x2 < b->x2 ? a->x2 : b->x2;
    const float y2 = a->y2 < b->y2 ? a->y2 : b->y2;

    const float iw = x2 - x1;
    const float ih = y2 - y1;

    if (iw <= 0.0f || ih <= 0.0f)
    {
        return 0.0f;
    }

    const float inter = iw * ih;
    const float area_a = (a->x2 - a->x1) * (a->y2 - a->y1);
    const float area_b = (b->x2 - b->x1) * (b->y2 - b->y1);
    const float uni    = area_a + area_b - inter;

    return uni > 0.0f ? inter / uni : 0.0f;
}

int yolov8npose_decode(const int8_t *out, yolov8npose_person_t *dst, int max_dst,
                       float *best_score)
{
    const int8_t *scores = ROW_PTR(out, ROW_SCORE);
    const int     q_thr  = quant_threshold(YOLOV8NPOSE_SCORE_THRESHOLD);

    candidate_t candidates[MAX_CANDIDATES];
    int         num_candidates = 0;
    int8_t      q_best = -128;

    if (out == NULL || dst == NULL || max_dst <= 0)
    {
        return 0;
    }

    /* Vorauswahl im quantisierten Raum: ein Vergleich je Anker, keine
     * Fliesskommaoperation auf den 2100 Werten. */
    for (int a = 0; a < ANCHORS; a++)
    {
        const int8_t q = scores[a];

        if (q > q_best)
        {
            q_best = q;
        }
        if (q >= q_thr)
        {
            num_candidates = insert_candidate(candidates, num_candidates, a, q);
        }
    }

    if (best_score != NULL)
    {
        *best_score = dequant(q_best);
    }

    /* Ueberlappungsfilterung, gierig ueber die nach Score sortierte Liste. */
    int count = 0;

    for (int i = 0; i < num_candidates && count < max_dst; i++)
    {
        yolov8npose_person_t person;

        person.score = dequant(candidates[i].q);
        decode_box(out, candidates[i].anchor, &person);

        int suppressed = 0;

        for (int j = 0; j < count; j++)
        {
            if (iou(&person, &dst[j]) > YOLOV8NPOSE_IOU_THRESHOLD)
            {
                suppressed = 1;
                break;
            }
        }

        if (!suppressed)
        {
            decode_joints(out, candidates[i].anchor, &person);
            dst[count] = person;
            count++;
        }
    }

    return count;
}

int yolov8npose_best_anchor(const int8_t *out)
{
    const int8_t *scores = ROW_PTR(out, ROW_SCORE);
    int    best = 0;
    int8_t q_best = scores[0];

    for (int a = 1; a < ANCHORS; a++)
    {
        if (scores[a] > q_best)
        {
            q_best = scores[a];
            best = a;
        }
    }

    return best;
}

void yolov8npose_print_raw_column(const int8_t *out, int anchor,
                                  const char *quelle)
{
    if (out == NULL || anchor < 0 || anchor >= ANCHORS)
    {
        return;
    }

    const int q_score = (int) ROW_PTR(out, ROW_SCORE)[anchor];

    printf("\r\n%s, staerkste Spalte %d von %d, Score roh %d = %.4f\r\n",
            quelle != NULL ? quelle : "Board", anchor, ANCHORS, q_score,
            (double) dequant((int8_t) q_score));

    printf("  Box   cx, cy, w, h : [%d, %d, %d, %d]\r\n",
            (int) ROW_PTR(out, 0)[anchor], (int) ROW_PTR(out, 1)[anchor],
            (int) ROW_PTR(out, 2)[anchor], (int) ROW_PTR(out, 3)[anchor]);
    printf("  Score              : %d\r\n", q_score);

    for (int k = 0; k < NJOINTS; k++)
    {
        const int row = ROW_JOINTS + 3 * k;

        printf("  Gelenk %2d  x=%5d y=%5d score=%5d\r\n", k,
                (int) ROW_PTR(out, row + 0)[anchor],
                (int) ROW_PTR(out, row + 1)[anchor],
                (int) ROW_PTR(out, row + 2)[anchor]);
    }

    printf("\r\nZeile 4 (Score), erste 20 von %d Werten:\r\n  [", ANCHORS);
    for (int a = 0; a < 20; a++)
    {
        printf("%d%s", (int) ROW_PTR(out, ROW_SCORE)[a], a < 19 ? ", " : "]\r\n");
    }
}

/* Rueckprojektion eines Punktes der Modellflaeche ins Originalbild. */
static void to_source(const yolov8npose_input_geometry_t *geo,
                      float x, float y, float *sx, float *sy)
{
    *sx = (x - (float) geo->pad_left) / geo->factor;
    *sy = (y - (float) geo->pad_top)  / geo->factor;
}

void yolov8npose_print_detections(const yolov8npose_person_t *persons, int count,
                                  const yolov8npose_input_geometry_t *geo)
{
    printf("\r\n=== YOLOv8n-Pose: %d Person(en) ueber Schwelle %.2f ===\r\n",
            count, (double) YOLOV8NPOSE_SCORE_THRESHOLD);

    for (int i = 0; i < count; i++)
    {
        const yolov8npose_person_t *p = &persons[i];

        printf("\r\nPerson %d   Score %.3f\r\n", i + 1, (double) p->score);
        printf("  Box  Modell 320x320 : x1=%7.1f  y1=%7.1f  x2=%7.1f  y2=%7.1f\r\n",
                (double) p->x1, (double) p->y1, (double) p->x2, (double) p->y2);

        if (geo != NULL)
        {
            float sx1, sy1, sx2, sy2;

            to_source(geo, p->x1, p->y1, &sx1, &sy1);
            to_source(geo, p->x2, p->y2, &sx2, &sy2);
            printf("       Originalbild  : x1=%7.1f  y1=%7.1f  x2=%7.1f  y2=%7.1f\r\n",
                    (double) sx1, (double) sy1, (double) sx2, (double) sy2);
        }

        if (geo != NULL)
        {
            printf("  Nr Zei Gelenk              Modell x,y        Original x,y      Score\r\n");
        }
        else
        {
            printf("  Nr Zei Gelenk              Modell x,y      Score\r\n");
        }

        for (int k = 0; k < NJOINTS; k++)
        {
            const yolov8npose_joint_t *j = &p->joints[k];
            const char sichtbar =
                    j->score >= YOLOV8NPOSE_JOINT_THRESHOLD ? ' ' : '?';

            printf("  %2d  %c  %-15s (%6.1f,%6.1f)", k, 'A' + k,
                    yolov8npose_joint_names[k], (double) j->x, (double) j->y);

            if (geo != NULL)
            {
                float sx, sy;

                to_source(geo, j->x, j->y, &sx, &sy);
                printf("  (%7.1f,%7.1f)", (double) sx, (double) sy);
            }

            printf("  %.2f %c\r\n", (double) j->score, sichtbar);
        }
    }

    if (geo != NULL)
    {
        printf("\r\n  Original = (Modell - Rand) / Faktor, Rand %u/%u px, Faktor %.4f\r\n",
                (unsigned) geo->pad_left, (unsigned) geo->pad_top,
                (double) geo->factor);
    }
    printf("  '?' markiert Gelenke unter der Sichtbarkeitsschwelle %.2f.\r\n",
            (double) YOLOV8NPOSE_JOINT_THRESHOLD);
}

/* --- ASCII-Grafik ------------------------------------------------------- */

#define CW  YOLOV8NPOSE_CANVAS_W
#define CH  YOLOV8NPOSE_CANVAS_H

/* Leinwand statisch: der Stack ist mit _Min_Stack_Size = 0x800 zu klein. */
static char canvas[CH][CW + 1];

static inline int to_col(float x)
{
    return (int) (x * (float) CW / (float) YOLOV8NPOSE_INPUT_WIDTH + 0.5f);
}

static inline int to_row(float y)
{
    return (int) (y * (float) CH / (float) YOLOV8NPOSE_INPUT_HEIGHT + 0.5f);
}

static void plot(int col, int row, char c)
{
    if (col >= 0 && col < CW && row >= 0 && row < CH)
    {
        canvas[row][col] = c;
    }
}

/* Zeichnet nur auf noch leere Zellen. So bleiben Rahmen und Gelenke sichtbar,
 * die Verbindungslinie endet an ihnen, statt sie zu ueberschreiben. */
static void plot_free(int col, int row, char c)
{
    if (col >= 0 && col < CW && row >= 0 && row < CH && canvas[row][col] == ' ')
    {
        canvas[row][col] = c;
    }
}

/* Ganzzahlige Linie nach Bresenham, ohne die beiden Endpunkte -- dort stehen
 * die Gelenkbuchstaben. */
static void draw_line(int c0, int r0, int c1, int r1, char c)
{
    const int dc = c1 > c0 ? c1 - c0 : c0 - c1;
    const int dr = r1 > r0 ? r1 - r0 : r0 - r1;
    const int sc = c0 < c1 ? 1 : -1;
    const int sr = r0 < r1 ? 1 : -1;

    int err = dc - dr;
    int col = c0;
    int row = r0;

    for (;;)
    {
        if (col == c1 && row == r1)
        {
            break;
        }

        const int err2 = 2 * err;

        if (err2 > -dr) { err -= dr; col += sc; }
        if (err2 <  dc) { err += dc; row += sr; }

        if (col == c1 && row == r1)
        {
            break;
        }
        plot_free(col, row, c);
    }
}

/* Rahmen einer Box, an den Raendern der Leinwand beschnitten. */
static void draw_box(int col1, int row1, int col2, int row2)
{
    for (int c = col1; c <= col2; c++)
    {
        plot(c, row1, '-');
        plot(c, row2, '-');
    }
    for (int r = row1; r <= row2; r++)
    {
        plot(col1, r, '|');
        plot(col2, r, '|');
    }

    plot(col1, row1, '+');
    plot(col2, row1, '+');
    plot(col1, row2, '+');
    plot(col2, row2, '+');
}

void yolov8npose_render_ascii(const yolov8npose_person_t *persons, int count)
{
    for (int r = 0; r < CH; r++)
    {
        for (int c = 0; c < CW; c++)
        {
            canvas[r][c] = ' ';
        }
        canvas[r][CW] = '\0';
    }

    for (int i = 0; i < count; i++)
    {
        const yolov8npose_person_t *p = &persons[i];

        const int col1 = to_col(p->x1);
        const int row1 = to_row(p->y1);
        const int col2 = to_col(p->x2);
        const int row2 = to_row(p->y2);

        draw_box(col1, row1, col2, row2);

        /* Personennummer eine Zelle innerhalb der oberen linken Ecke, damit
         * mehrere Boxen auseinanderzuhalten sind und der Rahmen ganz bleibt. */
        plot(col1 + 1, row1 + 1, (char) ('1' + (i % 9)));

        /* Skelett vor den Gelenken, damit die Buchstaben obenauf liegen. Eine
         * Kante wird nur gezogen, wenn beide Gelenke sichtbar sind -- eine
         * Linie zu einem unsicheren Gelenk zeigte eine Pose, die das Modell so
         * nicht behauptet. */
        for (int e = 0; e < NUM_EDGES; e++)
        {
            const yolov8npose_joint_t *a = &p->joints[skeleton[e][0]];
            const yolov8npose_joint_t *b = &p->joints[skeleton[e][1]];

            if (a->score >= YOLOV8NPOSE_JOINT_THRESHOLD
                    && b->score >= YOLOV8NPOSE_JOINT_THRESHOLD)
            {
                draw_line(to_col(a->x), to_row(a->y),
                          to_col(b->x), to_row(b->y), '.');
            }
        }

        /* Gelenke ueberzeichnen Rahmen und Linien. Grossbuchstabe = sichtbar,
         * Kleinbuchstabe = unter der Sichtbarkeitsschwelle. */
        for (int k = 0; k < NJOINTS; k++)
        {
            const yolov8npose_joint_t *j = &p->joints[k];
            const char c = j->score >= YOLOV8NPOSE_JOINT_THRESHOLD
                    ? (char) ('A' + k) : (char) ('a' + k);

            plot(to_col(j->x), to_row(j->y), c);
        }
    }

    printf("\r\n  Modellflaeche %ux%u px als %ux%u Zeichen"
           " (1 Zeichen = %.1f x %.1f px)\r\n",
            (unsigned) YOLOV8NPOSE_INPUT_WIDTH, (unsigned) YOLOV8NPOSE_INPUT_HEIGHT,
            (unsigned) CW, (unsigned) CH,
            (double) YOLOV8NPOSE_INPUT_WIDTH / CW,
            (double) YOLOV8NPOSE_INPUT_HEIGHT / CH);

    putchar('+');
    for (int c = 0; c < CW; c++)
    {
        putchar('-');
    }
    printf("+\r\n");

    for (int r = 0; r < CH; r++)
    {
        printf("|%s|\r\n", canvas[r]);
    }

    putchar('+');
    for (int c = 0; c < CW; c++)
    {
        putchar('-');
    }
    printf("+\r\n");

    /* Legende, drei Spalten. */
    printf("\r\n  Gelenke:\r\n");
    for (int k = 0; k < NJOINTS; k++)
    {
        printf("   %c %-16s", 'A' + k, yolov8npose_joint_names[k]);

        if ((k % 3) == 2 || k == NJOINTS - 1)
        {
            printf("\r\n");
        }
    }
    printf("   Kleinbuchstabe = Gelenk unter der Sichtbarkeitsschwelle,"
           " '.' = Skelettkante zwischen zwei sichtbaren Gelenken.\r\n");
}
