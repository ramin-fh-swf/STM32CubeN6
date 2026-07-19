/**
 ******************************************************************************
 * @file    app_x-cube-ai.c
 * @author  X-CUBE-AI C code generator
 * @brief   AI program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2026 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

/**
 * Description
 * Minimum template to show how to use the Neural-ART Embedded Client API
 *          Re-target of the printf function is out-of-scope.
 *
 *
 */

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

/* System headers */
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include <inttypes.h>

#include "app_x-cube-ai.h"
#include "bsp_ai.h"
#include "stai.h"
#include "npu_init.h"
#include "aiTestUtility.h"

/* USER CODE BEGIN includes */
#include "inference_conf.h"
#include "inter_hal.h"
#include "stm_inference_profiler.h"

#ifdef MODEL_YAMNET
#include "rm_ai_yamnet.h"
#endif

#ifdef MODEL_YAMNET_RAM
#include "rm_ai_yamnet_ram.h"
#endif

#ifdef MODEL_LENET5
#include "rm_ai_lenet5.h"
#endif

#ifdef MODEL_EFFICIENTNETV2B2
#include "rm_ai_efficientnetv2b3_300.h"
#endif

/* USER CODE END includes */

/* IO buffers ----------------------------------------------------------------*/

/* Input defs ----------------------------------------------------------------*/

/* Output defs ----------------------------------------------------------------*/

/* Activations buffers -------------------------------------------------------*/

/* Entry points --------------------------------------------------------------*/
LL_ATON_DECLARE_NAMED_NN_INSTANCE_AND_INTERFACE(Default)

static void parse_io_buffer(const LL_Buffer_InfoTypeDef *const buffer) //TODO: move to rm_ai
{
    if (buffer == NULL)
    {
        return;
    }
    printf("    Buffer name: %s\n", buffer->name);
    printf("    Buffer base address: 0x%" PRIxPTR "\n", buffer->addr_base.i);
    printf("    Buffer length: %lu\n", LL_Buffer_len(buffer));
    printf("    Buffer offset start: %lu\n", buffer->offset_start);
    printf("    Buffer offset end: %lu\n", buffer->offset_end);
    printf("    Buffer offset limit: %lu\n", buffer->offset_limit);
    printf("    Buffer is user allocated: %d\n", buffer->is_user_allocated);
    printf("    Buffer is param: %d\n", buffer->is_param);
    printf("    Buffer epoch: %d\n", buffer->epoch);
    printf("    Buffer batch: %lu\n", buffer->batch);
    printf("    Buffer mem shape: ");
    for (int i = 0; i < buffer->mem_ndims; i++)
    {
        printf(" %lu", buffer->mem_shape[i]);
    }
    printf("\n");
    printf("    Buffer mem ndims: %d\n", buffer->mem_ndims);
    printf("    Buffer chpos: %d\n", buffer->chpos);
    printf("    Buffer type: %d\n", buffer->type);
    printf("    Buffer Qm: %d\n", buffer->Qm);
    printf("    Buffer Qn: %d\n", buffer->Qn);
    printf("    Buffer Qunsigned: %d\n", buffer->Qunsigned);
    printf("    Buffer ndims: %d\n", buffer->ndims);
    printf("    Buffer nbits: %d\n", buffer->nbits);
    printf("    Buffer per_channel: %d\n", buffer->per_channel);
    printf("    Buffer shape: ");
    for (int i = 0; i < buffer->ndims; i++)
    {
        printf(" %lu", buffer->shape[i]);
    }
    printf("\n\n");
}

/*
 * Bootstrap
 */
static void init_ram(void)
{
#ifdef MODEL_YAMNET_RAM
    rm_ai_yamnet_ram_setup_sram();
#endif
}

int aiInit(void)
{
    LL_ATON_RT_RuntimeInit();
    LL_ATON_RT_Init_Network(&NN_Instance_Default);
    LL_ATON_RT_SetNetworkCallback(&NN_Instance_Default,
            stm_inference_profiler_on_epoch_event);

    const LL_Buffer_InfoTypeDef *inputBuffersInfos = LL_ATON_Input_Buffers_Info(
            &NN_Instance_Default);
    const LL_Buffer_InfoTypeDef *outputBuffersInfos =
            LL_ATON_Output_Buffers_Info(&NN_Instance_Default);

    uint8_t *buffer_in = (uint8_t*) LL_Buffer_addr_start(&inputBuffersInfos[0]);
    uint8_t *buffer_out = (uint8_t*) LL_Buffer_addr_start(&outputBuffersInfos[0]);

    printf("\n***************************************************\n");
    printf("Model name: %s\n", LL_ATON_DEFAULT_ORIGIN_MODEL_NAME);

#if defined(MODEL_WEIGHTS_SIZE_MB) && defined(MODEL_ACTIVATIONS_SIZE_MB) && defined(MODEL_MEMORY_USAGE_TOTAL_MB)
    printf("Model weights size : %.2fMiB\n", MODEL_WEIGHTS_SIZE_MB);
    printf("Model activations size: %.2fMiB\n", MODEL_ACTIVATIONS_SIZE_MB);
    printf("Model total size: %.2fMiB\n", MODEL_MEMORY_USAGE_TOTAL_MB);
#else
    printf("Model weights size : %.2fKiB\n", MODEL_WEIGHTS_SIZE_KB);
    printf("Model activations size: %.2fKiB\n", MODEL_ACTIVATIONS_SIZE_KB);
    printf("Model total size: %.2fKiB\n", MODEL_MEMORY_USAGE_TOTAL_KB);
#endif

    printf("Model weights placement: %s\n", MODEL_WEIGHTS_PLACEMENT);
    printf("Model activations placement: %s\n", MODEL_ACTIVATIONS_PLACEMENT);

    printf("\nParsing input buffer info:\n");
    printf("Input buffer address: 0x%p\n", (void*) buffer_in);
    parse_io_buffer(&inputBuffersInfos[0]);

    printf("Parsing output buffer info:\n");
    printf("Output buffer address: 0x%p\n", (void*) buffer_out);
    parse_io_buffer(&outputBuffersInfos[0]);

    return 0;
}

int aiDeinit(void)
{
    /* Deinitialize Neural-ART network instance */
    LL_ATON_RT_DeInit_Network(&NN_Instance_Default);
    LL_ATON_RT_RuntimeDeInit();
    return 0;
}

int acquire_and_process_data()
{
#ifdef MODEL_YAMNET
    rm_ai_yamnet_preprocess(&NN_Instance_Default);
#endif

#ifdef MODEL_YAMNET_RAM
    rm_ai_yamnet_ram_preprocess(&NN_Instance_Default);
#endif

#ifdef MODEL_LENET5
    rm_ai_lenet5_preprocess(&NN_Instance_Default);
#endif

#ifdef MODEL_EFFICIENTNETV2B2
    rm_ai_efficientnetv2b3_300_preprocess(&NN_Instance_Default);
#endif
    return 0;
}

/*
 * Run inference
 */
int aiRun()
{
    LL_ATON_RT_RetValues_t ll_aton_rt_ret = LL_ATON_RT_DONE;
    LL_ATON_RT_Reset_Network(&NN_Instance_Default);

    do
    {
        /* Execute first/next step */
        ll_aton_rt_ret = LL_ATON_RT_RunEpochBlock(&NN_Instance_Default);
        /* Wait for next event */
        if (ll_aton_rt_ret == LL_ATON_RT_WFE)
            LL_ATON_OSAL_WFE();
    } while (ll_aton_rt_ret != LL_ATON_RT_DONE);
    return 0;
}

int post_process()
{
#ifdef MODEL_YAMNET
    rm_ai_yamnet_postprocess(&NN_Instance_Default);
#endif

#ifdef MODEL_YAMNET_RAM
    rm_ai_yamnet_ram_postprocess(&NN_Instance_Default);
#endif

#ifdef MODEL_LENET5
    rm_ai_lenet5_postprocess(&NN_Instance_Default);
#endif

#ifdef MODEL_EFFICIENTNETV2B2
    rm_ai_efficientnetv2b3_300_postprocess(&NN_Instance_Default);
#endif
    return 0;
}

static void run_energy_measurement_phases(void)
{
#if defined (INFERENCE_CONF_WFE_DURATION_MS) && INFERENCE_CONF_WFE_DURATION_MS != 0U
    printf("Running WFE energy measurement phase...\r\n");
    stm_inference_profiler_set_pins_inference_start();
    inter_hal_set_system_wfe(INFERENCE_CONF_WFE_DURATION_MS);
    stm_inference_profiler_set_pins_inference_end();
    printf("WFE energy measurement phase completed!\r\n");
#endif
#if defined (INFERENCE_CONF_FULL_LOAD_DURATION_MS) && INFERENCE_CONF_FULL_LOAD_DURATION_MS != 0U
    printf("Running full load energy measurement phase...\r\n");
    stm_inference_profiler_set_pins_inference_start();
    inter_hal_set_system_full_load(INFERENCE_CONF_FULL_LOAD_DURATION_MS);
    stm_inference_profiler_set_pins_inference_end();
    printf("Full load energy measurement phase completed!\r\n\n");
#endif
}

const inter_hal_func_t obj =
{
    .init = cyclesCounterInit,
    .start_counter = cyclesCounterStart,
    .stop_counter = cyclesCounterEnd,
    .counter_val_to_float_ms = dwtCyclesToFloatMs,
    .get_time_ms = HAL_GetTick,
    .sleep_wfe_ms = stm_inference_profiler_sleep_in_wfe,
};

/* Entry points --------------------------------------------------------------*/

void STM32CubeAI_Studio_AI_Init(void)
{
    MX_UARTx_Init();
    aiPreInitialize();

    /* USER CODE BEGIN 5 */
    init_ram();
    inter_hal_create(&obj);
    inter_hal_init();

    stm_inference_profiler_init_inference_pins();
    stm_inference_profiler_init();
    aiInit();

    /* USER CODE END 5 */
}

volatile static inter_hal_statistic_inference_t report_hal;
static stm_profiler_statistic_t report_prof;

void STM32CubeAI_Studio_AI_Process(void)
{
    run_energy_measurement_phases();

    int inference_count =
        STM_PROFILER_MAX_SAMPLES < INFERENCE_CONF_INFERENZ_COUNT
            ? STM_PROFILER_MAX_SAMPLES
            : INFERENCE_CONF_INFERENZ_COUNT;

    printf("Running %d inferences for profiling...\r\n", inference_count);
    stm_inference_profiler_set_pins_inference_start();

    for (int i = 0; i < inference_count; i++)
    {
        /* 1 - Acquire, pre-process and fill the input buffers */
        acquire_and_process_data();

        /* 2 - Call inference engine */
        inter_hal_start_counter();
        stm_inference_profiler_begin();

        aiRun();

        stm_inference_profiler_end();
        float duration = inter_hal_get_counter(); //dur = dwtCyclesToFloatMs(cyclesCounterEnd());
        inter_hal_feed_statistic_inference_duration(duration);
    }

    stm_inference_profiler_set_pins_inference_end();
    printf("Running %d inferences for profiling completed!\r\n", inference_count);

    /* 3 - Post-process the predictions */
    post_process();

    /* --- Statistik nach allen Inferenzen berechnen --- */
    report_hal = inter_hal_finalize_statistic_inference_duration();
    stm_inference_profiler_finalize(&report_prof); /* war: einzelne Getter */

    /* --- Ausgabe (jetzt mit Median + Stddev, analog PSoC) --- */
    printf("\n***************************************************");
    printf("\r\nInference statistics (inter_hal):\r\n");
    printf("count=%u  min=%.3f  max=%.3f  avg=%.3f  mdn=%.3f  std=%.3f ms\r\n",
            (unsigned int) report_hal.inference_count, report_hal.min_time_ms,
            report_hal.max_time_ms, report_hal.average_time_ms,
            report_hal.median_time_ms, report_hal.stddev_time_ms);

    printf("\r\nInference statistics (stm_inference_profiler):\r\n");
    printf("Total : min=%.3f  max=%.3f  avg=%.3f  mdn=%.3f  std=%.3f ms\r\n",
            report_prof.total.min_ms, report_prof.total.max_ms,
            report_prof.total.average_ms, report_prof.total.median_ms,
            report_prof.total.stddev_ms);
    printf(
            "EC/NPU: min=%.3f  max=%.3f  avg=%.3f  mdn=%.3f  std=%.3f ms  (%u Bloecke)\r\n",
            report_prof.ec.min_ms, report_prof.ec.max_ms,
            report_prof.ec.average_ms, report_prof.ec.median_ms,
            report_prof.ec.stddev_ms,
            (unsigned int) report_prof.ec.block_count);
    printf(
            "Hybrid: min=%.3f  max=%.3f  avg=%.3f  mdn=%.3f  std=%.3f ms  (%u Bloecke)\r\n",
            report_prof.hybrid.min_ms, report_prof.hybrid.max_ms,
            report_prof.hybrid.average_ms, report_prof.hybrid.median_ms,
            report_prof.hybrid.stddev_ms,
            (unsigned int) report_prof.hybrid.block_count);
    printf(
            "SW    : min=%.3f  max=%.3f  avg=%.3f  mdn=%.3f  std=%.3f ms  (%u Bloecke)\r\n",
            report_prof.sw.min_ms, report_prof.sw.max_ms,
            report_prof.sw.average_ms, report_prof.sw.median_ms,
            report_prof.sw.stddev_ms,
            (unsigned int) report_prof.sw.block_count);
}

void STM32CubeAI_Studio_AI_Deinit(void)
{
    aiDeinit();
}

#ifdef __cplusplus
}
#endif
