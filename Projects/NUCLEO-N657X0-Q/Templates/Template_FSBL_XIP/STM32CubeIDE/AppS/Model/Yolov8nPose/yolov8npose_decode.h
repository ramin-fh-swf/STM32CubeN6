/*
 * yolov8npose_decode.h
 *
 * Auswertung des YOLOv8n-Pose-Ausgangstensors auf dem Ziel:
 *   1. Dekodierung  -- Boxen und 17 Gelenke aus dem int8-Tensor,
 *   2. Zahlenausgabe -- Koordinaten in Modellflaeche und Originalbild,
 *   3. ASCII-Grafik  -- Sichtpruefung ueber UART.
 *
 * Das Modul kennt nur den rohen Ausgangspuffer. Es haengt weder an der
 * ATON-Laufzeit noch an der HAL und ist daher auch auf dem Host uebersetzbar.
 *
 * Layout des Ausgangstensors (aus network.c, mem_shape = {1, 56, 2100},
 * chpos = CHPos_First): zeilenweise, Index = zeile * 2100 + anker.
 *   Zeile  0..3   cx, cy, w, h   auf die Kantenlaenge 320 normiert
 *   Zeile  4      Personen-Score
 *   Zeile  5..55  17 Gelenke zu je x, y, Score
 */

#ifndef YOLOV8NPOSE_DECODE_H
#define YOLOV8NPOSE_DECODE_H

#include <stdint.h>

/* Nur die Modellkonstanten. Welches Bild einkompiliert ist, geht das Modul
 * nichts an -- die Geometrie bekommt es als Zeiger uebergeben. */
#include "yolov8npose_model.h"

/* Obergrenze der gemeldeten Personen. Die Kandidatenliste vor der
 * Ueberlappungsfilterung ist unabhaengig davon auf 32 begrenzt. */
#define YOLOV8NPOSE_MAX_DETECTIONS   8

/* Schwellen, identisch zur Host-Referenz im Notebook. */
#define YOLOV8NPOSE_SCORE_THRESHOLD  (0.25f)
#define YOLOV8NPOSE_IOU_THRESHOLD    (0.45f)
#define YOLOV8NPOSE_JOINT_THRESHOLD  (0.50f)

/* Kantenlaenge der ASCII-Leinwand. 106 = 320/3 wie gefordert. Eine
 * Zeichenzelle ist etwa doppelt so hoch wie breit, deshalb halb so viele
 * Zeilen -- sonst erscheint die Pose vertikal gestreckt. */
#define YOLOV8NPOSE_CANVAS_W  (YOLOV8NPOSE_INPUT_WIDTH / 3)
#define YOLOV8NPOSE_CANVAS_H  (YOLOV8NPOSE_INPUT_HEIGHT / 6)

typedef struct {
    float x;      /* Pixel in der 320x320-Modellflaeche */
    float y;
    float score;  /* 0..1 */
} yolov8npose_joint_t;

typedef struct {
    float score;                                          /* Personen-Score 0..1 */
    float x1, y1, x2, y2;                                 /* Box, Pixel in 320x320 */
    yolov8npose_joint_t joints[YOLOV8NPOSE_NUM_JOINTS];   /* Pixel in 320x320 */
} yolov8npose_person_t;

/* Namen der Gelenke in COCO-Reihenfolge, Index 0..16. */
extern const char *const yolov8npose_joint_names[YOLOV8NPOSE_NUM_JOINTS];

/*
 * Dekodiert den Ausgangstensor.
 *
 * out     Zeiger auf YOLOV8NPOSE_OUTPUT_SIZE int8-Werte. Der Cache muss vom
 *         Aufrufer invalidiert sein, der Puffer wird nur gelesen.
 * dst     Zielfeld fuer die erkannten Personen, absteigend nach Score.
 * max_dst Groesse von dst.
 *
 * Rueckgabe: Anzahl der geschriebenen Personen.
 *
 * Ist best_score nicht NULL, wird dort der hoechste aufgetretene
 * Personen-Score abgelegt -- auch dann, wenn er unter der Schwelle liegt und
 * folglich keine Person gemeldet wird.
 */
int yolov8npose_decode(const int8_t *out, yolov8npose_person_t *dst, int max_dst,
                       float *best_score);

/*
 * Anker mit dem hoechsten Personen-Score, ohne Schwelle und ohne
 * Ueberlappungsfilterung. Rueckgabe: Spaltenindex 0..2099.
 */
int yolov8npose_best_anchor(const int8_t *out);

/*
 * Gibt eine Spalte des Ausgangstensors als rohe int8-Werte aus, im Format des
 * Auszugs aus Abschnitt 7 des Notebooks. Damit lassen sich Board und
 * Host-Referenz Zahl gegen Zahl vergleichen, ohne dass die Dequantisierung
 * dazwischen liegt und Abweichungen verwischt.
 *
 * quelle  Name des Quellbildes fuer die Kopfzeile, darf NULL sein.
 */
void yolov8npose_print_raw_column(const int8_t *out, int anchor,
                                  const char *quelle);

/*
 * Gibt Box und Gelenke als Zahlentabelle aus.
 *
 * geo  Auffuellgeometrie des Eingangsbildes fuer die Rueckprojektion ins
 *      Originalbild. NULL unterdrueckt die Originalbildspalten.
 */
void yolov8npose_print_detections(const yolov8npose_person_t *persons, int count,
                                  const yolov8npose_input_geometry_t *geo);

/*
 * Zeichnet Boxen und Gelenke als ASCII-Grafik der Modellflaeche.
 */
void yolov8npose_render_ascii(const yolov8npose_person_t *persons, int count);

#endif /* YOLOV8NPOSE_DECODE_H */
