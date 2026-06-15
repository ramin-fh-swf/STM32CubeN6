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
#include "efficientnetv2b3_300_inputs.h"

#define EFFICIENTNETV2B3_300_OUTPUT_NEUTON_COUNT 1000u

typedef struct best_idx
{
    int id_best_0;
    int id_best_1;
    int id_best_2;
} best_idx_t;

static best_idx_t get_prediction(const float *output, int len)
{
    best_idx_t best =
    { -1, -1, -1 };
    float v0 = -FLT_MAX, v1 = -FLT_MAX, v2 = -FLT_MAX;

    for (int i = 0; i < len; i++)
    {
        float v = output[i];

        if (v > v0)
        {
            v2 = v1;
            best.id_best_2 = best.id_best_1;
            v1 = v0;
            best.id_best_1 = best.id_best_0;
            v0 = v;
            best.id_best_0 = i;
        }
        else if (v > v1)
        {
            v2 = v1;
            best.id_best_2 = best.id_best_1;
            v1 = v;
            best.id_best_1 = i;
        }
        else if (v > v2)
        {
            v2 = v;
            best.id_best_2 = i;
        }
    }

    return best;
}

void rm_ai_efficientnetv2b3_300_preprocess(
        const NN_Instance_TypeDef *nn_instance)
{
    const LL_Buffer_InfoTypeDef *inputBuffersInfos = LL_ATON_Input_Buffers_Info(
            nn_instance);
    uint8_t *buffer_in = (uint8_t*) LL_Buffer_addr_start(inputBuffersInfos);
    uint32_t buffer_len = LL_Buffer_len(inputBuffersInfos); // should be 3*300*300 = 270000 for efficientnetv2b3_300

    assert(buffer_len == 3 * 300 * 300);
    memset(buffer_in, 0, buffer_len);
    memcpy(buffer_in, efficientnetv2_input_3, buffer_len);

    SCB_CleanDCache_by_Addr((uint32_t*) buffer_in, buffer_len);

}

void rm_ai_efficientnetv2b3_300_postprocess(
        const NN_Instance_TypeDef *nn_instance)
{
    const LL_Buffer_InfoTypeDef *outputBuffersInfos =
            LL_ATON_Output_Buffers_Info(nn_instance);
    uint8_t *buffer_out = (uint8_t*) LL_Buffer_addr_start(outputBuffersInfos);

    static float output_value[EFFICIENTNETV2B3_300_OUTPUT_NEUTON_COUNT];
    SCB_InvalidateDCache_by_Addr((uint32_t*) buffer_out, sizeof(output_value));
    memcpy(output_value, buffer_out, sizeof(output_value));

    const best_idx_t best_ids = get_prediction(output_value,
            EFFICIENTNETV2B3_300_OUTPUT_NEUTON_COUNT);
    printf("=== Top-3 Prediction ===\n");
    printf("  1. Class %4d  (%.1f %%)\n", best_ids.id_best_0,
            100.0f * output_value[best_ids.id_best_0]);
    printf("  2. Class %4d  (%.1f %%)\n", best_ids.id_best_1,
            100.0f * output_value[best_ids.id_best_1]);
    printf("  3. Class %4d  (%.1f %%)\n", best_ids.id_best_2,
            100.0f * output_value[best_ids.id_best_2]);
}
