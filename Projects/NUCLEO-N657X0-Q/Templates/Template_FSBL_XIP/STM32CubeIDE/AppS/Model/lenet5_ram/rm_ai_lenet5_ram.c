/*
 * rm_ai_lenet5.c
 *
 *  Created on: 19.05.2026
 *      Author: rama
 */

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <string.h>

#include "app_x-cube-ai.h"
#include "bsp_ai.h"
#include "stai.h"
#include "npu_init.h"

#include "rm_ai.h"
#include "mnist_input_label.h"
#include "network_atonbuf.AXISRAM3.h"

void rm_ai_lenet5_ram_setup_sram()
{
    /* Copy the model weights from flash into RAM */
    memcpy((void *)G_AXISRAM3_ADDR, g_axisram3, g_axisram3_len);
    #if (__DCACHE_PRESENT == 1U)
    SCB_CleanDCache_by_Addr((void *)G_AXISRAM3_ADDR, g_axisram3_len);
    #endif
}

static int get_prediction(int8_t *output, int len)
{
    int max_idx = 0;
    int8_t max_val = output[0];

    for (int i = 1; i < len; i++)
    {
        if (output[i] > max_val)
        {
            max_val = output[i];
            max_idx = i;
        }
    }

    return max_idx;
}

void rm_ai_lenet5_ram_preprocess(const NN_Instance_TypeDef *nn_instance)
{
    const LL_Buffer_InfoTypeDef *inputBuffersInfos = LL_ATON_Input_Buffers_Info(nn_instance);
    uint8_t *buffer_in = (uint8_t *)LL_Buffer_addr_start(inputBuffersInfos);
    uint32_t buffer_len = LL_Buffer_len(inputBuffersInfos); // should be 28*28 = 784 for lenet5

    memset(buffer_in, 0, buffer_len);
    memcpy(buffer_in, mnist_input_label_8, 784);
    SCB_CleanDCache_by_Addr((uint32_t *)buffer_in, buffer_len);
}

void rm_ai_lenet5_ram_postprocess(const NN_Instance_TypeDef *nn_instance)
{
    const LL_Buffer_InfoTypeDef *outputBuffersInfos = LL_ATON_Output_Buffers_Info(nn_instance);
    uint8_t *buffer_out = (uint8_t *)LL_Buffer_addr_start(outputBuffersInfos);

    static int8_t output_value[10];
    SCB_InvalidateDCache_by_Addr((uint32_t *)buffer_out, 32);
    memcpy(output_value, buffer_out, sizeof(output_value));
    for (int i = 0; i < 10; i++)
    {
        printf("Output value %d: %d\n", i, (int8_t)output_value[i]);
    }

    int pred = get_prediction(output_value, 10);
    printf("Predicted class: %d\n\n", pred);
}
