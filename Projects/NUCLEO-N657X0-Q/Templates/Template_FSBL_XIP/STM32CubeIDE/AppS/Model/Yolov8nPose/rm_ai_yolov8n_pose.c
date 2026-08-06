#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include <assert.h>
#include <float.h>   // FLT_MAX

#include "app_x-cube-ai.h"
#include "bsp_ai.h"
#include "stai.h"
#include "npu_init.h"

#include "rm_ai.h"

/* Change the input picture y including the respective header file */
// #include "yolov8npose_liana_9_320_int8.h"
#include "yolov8npose_bild_000000000885_many_320_int8.h"
#include "yolov8npose_decode.h"

void rm_ai_yolov8n_pose_preprocess(const NN_Instance_TypeDef *nn_instance)
{
    const LL_Buffer_InfoTypeDef *inputBuffersInfos = LL_ATON_Input_Buffers_Info(nn_instance);
    uint8_t *buffer_in = (uint8_t*) LL_Buffer_addr_start(inputBuffersInfos);
    uint32_t buffer_len = LL_Buffer_len(inputBuffersInfos); //should be 96*64 = 6144 for yamnet

    /* Die folgenden Funktionen haben in der Vergangenheit zu einem Hardfault geführt.
     * Problem war die fehlende MPU_Config().
     * Für mehr Infos sehe die Doku: nucleo-n657x0-q/projects/Template_FSBL_XIP/README.md  */
    /* Kein memset() davor: das memcpy() schreibt dieselbe Länge und überschreibt
     * es vollständig. */
    memcpy(buffer_in, YOLOV8NPOSE_ACTIVE_INPUT, buffer_len);
    SCB_CleanDCache_by_Addr((uint32_t*) buffer_in, buffer_len);
}


void rm_ai_yolov8n_pose_postprocess(const NN_Instance_TypeDef *nn_instance)
{
    const LL_Buffer_InfoTypeDef *outputBuffersInfos = LL_ATON_Output_Buffers_Info(nn_instance);
    int8_t *buffer_out = (int8_t*) LL_Buffer_addr_start(outputBuffersInfos);

    assert(LL_Buffer_len(outputBuffersInfos) == YOLOV8NPOSE_OUTPUT_SIZE);

    /* Der Ausgangspuffer liegt in npuRAM5 und wird nur gelesen. Nach der
     * Invalidierung arbeitet die Dekodierung direkt darauf, eine Kopie von
     * 117600 B in den Arbeitsspeicher entfaellt. */
    SCB_InvalidateDCache_by_Addr((uint32_t*) buffer_out, YOLOV8NPOSE_OUTPUT_SIZE);

    /* Rohe int8-Werte der staerksten Spalte, im Format des Auszugs aus
     * Abschnitt 7 von yolo_v8n_pose_image_to_c_array.ipynb. Der Vergleich mit
     * der Host-Referenz laeuft damit auf den Werten, die auch im
     * Ausgangspuffer stehen, und nicht auf dequantisierten. */
    yolov8npose_print_raw_column(buffer_out,
                                 yolov8npose_best_anchor(buffer_out),
                                 YOLOV8NPOSE_ACTIVE_NAME);

    /* static: 8 * 224 B = 1792 B, der Stack ist mit _Min_Stack_Size = 0x800
     * zu klein. */
    static yolov8npose_person_t persons[YOLOV8NPOSE_MAX_DETECTIONS];
    float best_score = 0.0f;

    const int count = yolov8npose_decode(buffer_out, persons,
            YOLOV8NPOSE_MAX_DETECTIONS, &best_score);

    if (count == 0)
    {
        printf("\r\n=== YOLOv8n-Pose: keine Person ueber Schwelle %.2f"
               " (hoechster Score %.3f) ===\r\n",
                (double) YOLOV8NPOSE_SCORE_THRESHOLD, (double) best_score);
        return;
    }

    /* Die Ausgabe laeuft ausserhalb des Messfensters: post_process() wird in
     * STM32CubeAI_Studio_AI_Process() erst nach stm_inference_profiler_set_
     * pins_inference_end() aufgerufen. Latenz und Energie bleiben unberuehrt. */
    yolov8npose_print_detections(persons, count, &YOLOV8NPOSE_ACTIVE_GEOMETRY);
    yolov8npose_render_ascii(persons, count);
}
