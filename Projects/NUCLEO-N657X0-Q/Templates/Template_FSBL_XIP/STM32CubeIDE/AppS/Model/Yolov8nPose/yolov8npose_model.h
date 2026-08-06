/*
 * yolov8npose_model.h
 *
 * Modellkonstanten fuer YOLOv8n-Pose (voll-int8),
 * erzeugt von yolo_v8n_pose_image_to_c_array.ipynb.
 * Modell: Yolov8nPose.tflite
 * Verwendbar auf PSoC Edge E84 (Ethos-U55) und STM32N657 (Neural-ART).
 * Erstellt am: 2026-08-05 22:24:31
 *
 * Eingangsform  : (320, 320, 3)   // NHWC, RGB
 * Ausgangsform  : (56, 2100)   // 117600 Byte, zeilenweise
 *
 * Zeilenbelegung des Ausgangs je Kandidat:
 *   0..3   cx, cy, w, h   normiert auf die Kantenlaenge der Flaeche
 *   4      Personen-Score
 *   5..55  17 Gelenke zu je x, y, Score
 *
 * Die Bilddaten stehen nicht hier, sondern je Bild in einem eigenen
 * Header yolov8npose_<bild>_<kantenlaenge>_int8.h.
 */

#ifndef YOLOV8NPOSE_MODEL_H
#define YOLOV8NPOSE_MODEL_H

#include <stdint.h>

#define YOLOV8NPOSE_INPUT_HEIGHT     320
#define YOLOV8NPOSE_INPUT_WIDTH      320
#define YOLOV8NPOSE_INPUT_CHANNELS   3
#define YOLOV8NPOSE_INPUT_SIZE       307200  /* 320*320*3, NHWC, RGB */
#define YOLOV8NPOSE_PAD_VALUE        114  /* Grauwert der Raender, vor der Quantisierung */

#define YOLOV8NPOSE_OUTPUT_ROWS      56  /* 4 Box + 1 Score + 3*17 Gelenkwerte */
#define YOLOV8NPOSE_OUTPUT_ANCHORS   2100  /* Raster 40x40 + 20x20 + 10x10 */
#define YOLOV8NPOSE_OUTPUT_SIZE      117600
#define YOLOV8NPOSE_NUM_JOINTS       17

/* Quantisierung, ausgelesen aus der Modelldatei.
 *   Eingang:  q    = round(x / scale) + zero_point
 *             hier gleichwertig zu q = pixel - 128,
 *             die Firmware braucht also nur eine Verschiebung
 *   Ausgang:  wert = (q - zero_point) * scale   // auf dem Board */
#define YOLOV8NPOSE_INPUT_SCALE        (0.003921568859368563f)
#define YOLOV8NPOSE_INPUT_ZERO_POINT   (-128)
#define YOLOV8NPOSE_OUTPUT_SCALE       (0.0056150914169847965f)
#define YOLOV8NPOSE_OUTPUT_ZERO_POINT  (-117)

/* Geometrie der Auffuellung, je Bild einmal belegt. Die Modellausgabe
 * ist auf die Kantenlaenge normiert, ein Punkt kommt also so ins
 * Originalbild zurueck:
 *   x_flaeche = x_modell * YOLOV8NPOSE_INPUT_WIDTH
 *   x_orig    = (x_flaeche - pad_left) / factor
 *   y_orig    = (y_flaeche - pad_top)  / factor */
typedef struct {
    uint16_t src_width;    /* Breite des Originalbildes      */
    uint16_t src_height;   /* Hoehe des Originalbildes       */
    uint16_t new_width;    /* Breite nach der Verkleinerung  */
    uint16_t new_height;   /* Hoehe nach der Verkleinerung   */
    uint16_t pad_left;     /* Rand links, gleich dem rechten */
    uint16_t pad_top;      /* Rand oben, gleich dem unteren  */
    float    factor;       /* Verkleinerungsfaktor           */
} yolov8npose_input_geometry_t;

#endif /* YOLOV8NPOSE_MODEL_H */