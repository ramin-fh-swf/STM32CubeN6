
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

#include "app_x-cube-ai.h"
#include "bsp_ai.h"
#include "stai.h"
#include "npu_init.h"



/* USER CODE BEGIN includes */
#include <mnist_input_label_0.h>
#include <mnist_input_label_3.h>
#include <mnist_input_label_5.h>
#include <mnist_input_label_7.h>
#include <mnist_input_label_9.h>

/* USER CODE END includes */

/* IO buffers ----------------------------------------------------------------*/


/* Input defs ----------------------------------------------------------------*/

/**

// Array to store the data of the input tensor
stai_ptr data_ins[] = {
};
*/

/* Output defs ----------------------------------------------------------------*/

/**

// c-array to store the data of the output tensor
stai_ptr data_outs[] = {
};
*/




/* Activations buffers -------------------------------------------------------*/


/* Entry points --------------------------------------------------------------*/
LL_ATON_DECLARE_NAMED_NN_INSTANCE_AND_INTERFACE(Default)
uint8_t *buffer_in;
uint8_t *buffer_out;

/*
 * Bootstrap
 */
int aiInit(void) {
    /* Retreive the start address of the input and output
     * buffers (reserved in the activation buffer)
     */
    const LL_Buffer_InfoTypeDef * inputBuffersInfos = LL_ATON_Input_Buffers_Info(&NN_Instance_Default);
    const LL_Buffer_InfoTypeDef * outputBuffersInfos = LL_ATON_Output_Buffers_Info(&NN_Instance_Default);

    printf("mem_ndims: %d\n", inputBuffersInfos[0].mem_ndims);

    for (int i = 0; i < inputBuffersInfos[0].mem_ndims; i++) {
        printf("mem_shape[%d] = %lu\n", i, inputBuffersInfos[0].mem_shape[i]);
    }

    printf("ndims: %d\n", inputBuffersInfos[0].ndims);

    for (int i = 0; i < inputBuffersInfos[0].ndims; i++) {
        printf("shape[%d] = %lu\n", i, inputBuffersInfos[0].shape[i]);
    }

    printf("chpos: %d\n", inputBuffersInfos[0].chpos);
    printf("nbits: %d\n", inputBuffersInfos[0].nbits);
    printf("Qunsigned: %d\n", inputBuffersInfos[0].Qunsigned);

    buffer_in = (uint8_t *)LL_Buffer_addr_start(&inputBuffersInfos[0]);
    buffer_out = (uint8_t *)LL_Buffer_addr_start(&outputBuffersInfos[0]);
    LL_ATON_RT_RuntimeInit();
    LL_ATON_RT_Init_Network(&NN_Instance_Default);
  return 0;
}

int aiDeinit(void) {
  /* Deinitialize Neural-ART network instance */
  LL_ATON_RT_DeInit_Network(&NN_Instance_Default);
  LL_ATON_RT_RuntimeDeInit();
  return 0;
}

/*
 * Run inference
 */
int aiRun() {

    LL_ATON_RT_RetValues_t ll_aton_rt_ret = LL_ATON_RT_DONE;
    LL_ATON_RT_Reset_Network(&NN_Instance_Default);

    do {
      /* Execute first/next step */
      ll_aton_rt_ret = LL_ATON_RT_RunEpochBlock(&NN_Instance_Default);
      /* Wait for next event */
      if (ll_aton_rt_ret == LL_ATON_RT_WFE)
        LL_ATON_OSAL_WFE();
    } while (ll_aton_rt_ret != LL_ATON_RT_DONE);
  return 0;
}


int acquire_and_process_data()
{
  // RM: following code might be wrong!!
  static const uint8_t max_labels = 5;
  static uint8_t counter = 0;
  memset(buffer_in, 0, 848);

  if (counter == 0) {
	  memcpy(buffer_in, mnist_input_label_0, 784);
  } else if (counter == 1) {
	  memcpy(buffer_in, mnist_input_label_3, 784);
  } else if ( counter == 2 ) {
	  memcpy(buffer_in, mnist_input_label_5, 784);
  } else if ( counter == 3 ) {
	  memcpy(buffer_in, mnist_input_label_7, 784);
  } else if ( counter == 4 ) {
	  memcpy(buffer_in, mnist_input_label_9, 784);
  } else {
	  // ERROR
  }

  SCB_CleanDCache_by_Addr((uint32_t*)buffer_in, 848);
  counter++;
  if (counter >= max_labels) {
	  counter = 0;
  }

  /*
  printf("buffer_in[%d] = %d\n", 201, (int8_t)buffer_in[201]);
  printf("buffer_in[%d] = %d\n", 202, (int8_t)buffer_in[202]);
  printf("buffer_in[%d] = %d\n", 203, (int8_t)buffer_in[203]);
   */

  /* fill the inputs of the c-model
  for (int idx=0; idx < STAI_NETWORK_IN_NUM; idx++ )
  {
      stai_input[idx] = ....
  }

  */
  return 0;
}

int get_prediction(int8_t *output, int len)
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

int post_process()
{
  static int8_t output_value[10];
  SCB_InvalidateDCache_by_Addr((uint32_t*)buffer_out, 32);
  memcpy(output_value, buffer_out, sizeof(output_value));
  for(int i=0; i<10; i++)
  {
    printf("Output value %d: %d\n", i, (int8_t)output_value[i]);
  }

  int pred = get_prediction(output_value, 10);

  printf("Predicted class: %d\n\n", pred);

  /* process the predictions
  for (int idx=0; idx < STAI_NETWORK_OUT_NUM; idx++ )
  {
      stai_output[idx] = ....
  }

  */
  return 0;
}



/*
 * Example of main loop function
 */
void main_loop() {
  while (1) {
    /* 1 - Acquire, pre-process and fill the input buffers */
    acquire_and_process_data();

    /* 2 - Call inference engine */
    aiRun();

    /* 3 - Post-process the predictions */
    post_process();
  }
}


/* Entry points --------------------------------------------------------------*/



void STM32CubeAI_Studio_AI_Init(void)
{
    MX_UARTx_Init();
    aiPreInitialize();
    /* USER CODE BEGIN 5 */
    aiInit();

    const LL_Buffer_InfoTypeDef * inputBuffersInfos = LL_ATON_Input_Buffers_Info(&NN_Instance_Default);
    const LL_Buffer_InfoTypeDef * outputBuffersInfos = LL_ATON_Output_Buffers_Info(&NN_Instance_Default);
    printf("Input buffer address: 0x%p\n", (void *)LL_Buffer_addr_start(&inputBuffersInfos[0]));
    printf("Output buffer address: 0x%p\n", (void *)LL_Buffer_addr_start(&outputBuffersInfos[0]));

    /* USER CODE END 5 */
}

void STM32CubeAI_Studio_AI_Process(void)
{
    /* 1 - Acquire, pre-process and fill the input buffers */
    acquire_and_process_data();

    /* 2 - Call inference engine */
    aiRun();

    /* 3 - Post-process the predictions */
    post_process();
}

void STM32CubeAI_Studio_AI_Deinit(void)
{
    aiDeinit();
}


#ifdef __cplusplus
}
#endif
