/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "rm_init.h"
#include "app_x-cube-ai.h"
#include "inference_conf.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
CACHEAXI_HandleTypeDef hcacheaxi;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static void MPU_Config(void);

/* USER CODE BEGIN PFP */
void Error_Handler(void);
static void MX_CACHEAXI_Init(void);
static void MX_GPIO_Init(void);
static void SystemIsolation_Config(void);
static void SystemClock_Config_AppCpu(void);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
  /* System clock already configured, simply SystemCoreClock init */
  SystemCoreClockUpdate();
  /* USER CODE END 1 */

  /* MPU Configuration--------------------------------------------------------*/
  MPU_Config();

  /* Enable the CPU Cache */

  /* Enable I-Cache---------------------------------------------------------*/
  SCB_EnableICache();

  /* Enable D-Cache---------------------------------------------------------*/
  SCB_EnableDCache();

  /* MCU Configuration--------------------------------------------------------*/
  HAL_Init();

  /* USER CODE BEGIN Init */
  SystemClock_Config_AppCpu();
  rm_init_uart();
  printf("######## AppS ########\n");

  rm_init_calculate_print_freqs();
  MX_GPIO_Init();

  /* CACHEAXI is AXISRAM 7 - will be used to cache model content from Flash into RAM */
  MX_CACHEAXI_Init();

  /* Initialize LED1 */
  BSP_LED_Init(LED_GREEN);
  SystemIsolation_Config();
  /* USER CODE END Init */

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  STM32CubeAI_Studio_AI_Init();
  printf("App init done ...\n");

  STM32CubeAI_Studio_AI_Process();

  while (1)
  {
    /* Toggle LED1 every 250ms */
    HAL_Delay(250);
    // BSP_LED_Toggle(LED_GREEN);
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/* USER CODE BEGIN 4 */

/**
  * @brief CACHEAXI Initialization Function
  * @param None
  * @retval None
  */
static void MX_CACHEAXI_Init(void)
{

  /* USER CODE BEGIN CACHEAXI_Init 0 */

  /* USER CODE END CACHEAXI_Init 0 */

  /* USER CODE BEGIN CACHEAXI_Init 1 */

  /* USER CODE END CACHEAXI_Init 1 */
  hcacheaxi.Instance = CACHEAXI;
  if (HAL_CACHEAXI_Init(&hcacheaxi) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CACHEAXI_Init 2 */
  //npu_cache_enable();
  printf(" - CACHEAXI successfully initialized\n");

  /* USER CODE END CACHEAXI_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOO_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPION_CLK_ENABLE();

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/**
  * @brief RIF Initialization Function
  * @param None
  * @retval None
  */
static void SystemIsolation_Config(void)
{

  /* USER CODE BEGIN RIF_Init 0 */

  /* USER CODE END RIF_Init 0 */

  /* set all required IPs as secure privileged */
  __HAL_RCC_RIFSC_CLK_ENABLE();

  /*RIMC configuration*/
  RIMC_MasterConfig_t RIMC_master = {0};
  RIMC_master.MasterCID = RIF_CID_1;
  RIMC_master.SecPriv = RIF_ATTRIBUTE_SEC | RIF_ATTRIBUTE_NPRIV;
  HAL_RIF_RIMC_ConfigMasterAttributes(RIF_MASTER_INDEX_ETH1, &RIMC_master);

  /* RIF-Aware IPs Config */

  /* set up GPIO configuration */
  HAL_GPIO_ConfigPinAttributes(GPIOA,GPIO_PIN_5,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOA,GPIO_PIN_7,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOA,GPIO_PIN_10,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOA,GPIO_PIN_11,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOB,GPIO_PIN_0,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOB,GPIO_PIN_3,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOB,GPIO_PIN_6,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOB,GPIO_PIN_7,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOB,GPIO_PIN_10,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOB,GPIO_PIN_11,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOC,GPIO_PIN_1,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOD,GPIO_PIN_2,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOD,GPIO_PIN_10,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOE,GPIO_PIN_3,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOE,GPIO_PIN_5,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOE,GPIO_PIN_6,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOH,GPIO_PIN_9,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPION,GPIO_PIN_7,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOO,GPIO_PIN_5,GPIO_PIN_SEC|GPIO_PIN_NPRIV);

  /* USER CODE BEGIN RIF_Init 1 */

  /* USER CODE END RIF_Init 1 */
  /* USER CODE BEGIN RIF_Init 2 */

  /* USER CODE END RIF_Init 2 */

}

/* USER CODE END 4 */

 /* MPU Configuration */

__attribute__((unused)) void MPU_Config(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct = {0};
  MPU_Attributes_InitTypeDef MPU_AttributesInit = {0};
  uint32_t primask_bit = __get_PRIMASK();
  __disable_irq();

  /* Disables the MPU */
  HAL_MPU_Disable();

  /** Initializes and configures the Region 0 and the memory to be protected
  */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER0;
  MPU_InitStruct.BaseAddress = __NON_CACHEABLE_SECTION_BEGIN;
  MPU_InitStruct.LimitAddress = __NON_CACHEABLE_SECTION_END;
  MPU_InitStruct.AttributesIndex = MPU_ATTRIBUTES_NUMBER0;
  MPU_InitStruct.AccessPermission = MPU_REGION_ALL_RW;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;
  MPU_InitStruct.DisablePrivExec = MPU_PRIV_INSTRUCTION_ACCESS_ENABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /** Initializes and configures the Attribute 0 and the memory to be protected
  */
  MPU_AttributesInit.Number = MPU_ATTRIBUTES_NUMBER0;
  MPU_AttributesInit.Attributes = INNER_OUTER(MPU_NOT_CACHEABLE);

  HAL_MPU_ConfigMemoryAttributes(&MPU_AttributesInit);
  /* Enables the MPU */
  HAL_MPU_Enable(MPU_HFNMI_PRIVDEF);

  /* Exit critical section to lock the system and avoid any issue around MPU mechanism */
  __set_PRIMASK(primask_bit);

}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config_AppCpu(void) // CPU + optional sysb/sysc/sysd, App-seitig
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /* WICHTIG: PLL1/PLL2 (2400 MHz, von der FSBL gesetzt) bleiben unberührt.
   * Einzige PLL-Aktion hier ist das Einschalten der unabhängigen PLL3 (nur Config 4);
   * dabei stehen PLL1/2/4 auf RCC_PLL_NONE -> HAL_RCC_OscConfig fasst sie nicht an
   * (die HAL verweigert zudem das Rekonfigurieren einer in Benutzung befindlichen PLL).
   * Kein Reconfigure von PLL1/PLL2 -> kein Glitch der laufenden XIP-/CPU-Takte. */

  /* --- VCORE-Overdrive für Maximalfrequenz (nur wenn die Config es verlangt) ---
   * Reihenfolge zwingend: erst Spannung hoch, DANN Frequenz.
   *   1) PB12 high -> externer SMPS liefert höhere VCORE (Board MB1940 ab Rev C01)
   *   2) VOS SCALE0 -> interner Betriebspunkt "highest performance" (+ VOSRDY-Handshake)
   * HAL_PWREx_ConfigSupply(EXTERNAL) ist bereits von der FSBL gesetzt und persistiert. */
#if defined(INFERENCE_CONF_VCORE_OVERDRIVE) && (INFERENCE_CONF_VCORE_OVERDRIVE == 1U)
  BSP_SMPS_Init(SMPS_VOLTAGE_OVERDRIVE);
  HAL_Delay(1);   /* externer SMPS muss VCORE hochrampen, bevor VOS0 angefordert wird
                   * (sonst latcht der ACTVOS-Übergang nicht -> bleibt SCALE1) */
  __HAL_RCC_PWR_CLK_ENABLE();
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE0) != HAL_OK)
  {
    Error_Handler();
  }
#endif

  /* --- Dedizierte NPU-PLL (PLL3 = 2000 MHz) einschalten, nur wenn die Config sie nutzt ---
   * Nur PLL3 wird angefasst; PLL1/PLL2/PLL4 = RCC_PLL_NONE -> unberührt (kein XIP-Glitch).
   * Muss vor dem Trio stehen (OscConfig wartet auf PLL3-Lock), damit IC6 <- PLL3 gültig ist. */
#if defined(IC6_ENABLE_PLL3) && (IC6_ENABLE_PLL3 == 1U)
  {
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_NONE; /* HSI etc. nicht antasten     */
    RCC_OscInitStruct.PLL1.PLLState  = RCC_PLL_NONE;            /* laufende PLLs in Ruhe lassen */
    RCC_OscInitStruct.PLL2.PLLState  = RCC_PLL_NONE;
    RCC_OscInitStruct.PLL4.PLLState  = RCC_PLL_NONE;
    RCC_OscInitStruct.PLL3.PLLState  = RCC_PLL_ON;              /* nur PLL3 = 2000 MHz */
    RCC_OscInitStruct.PLL3.PLLSource = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL3.PLLM = 4;    /* 64 MHz / 4 = 16 MHz PLL-Eingang */
    RCC_OscInitStruct.PLL3.PLLN = 125;  /* 16 MHz * 125 = 2000 MHz VCO     */
    RCC_OscInitStruct.PLL3.PLLFractional = 0;
    RCC_OscInitStruct.PLL3.PLLP1 = 1;
    RCC_OscInitStruct.PLL3.PLLP2 = 1;   /* 2000 / (1*1) = 2000 MHz         */
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
      Error_Handler();
    }
  }
#endif

  /* --- Dedizierte NPURAM-PLL (PLL4 = 1800 MHz) einschalten, nur wenn die Config sie nutzt ---
   * Gleiche Sicherheit wie PLL3: nur PLL4 an, PLL1/2/3 = RCC_PLL_NONE -> unberührt.
   * (PLL3 kann parallel schon laufen; NONE bedeutet 'nicht antasten'.) */
#if defined(IC11_ENABLE_PLL4) && (IC11_ENABLE_PLL4 == 1U)
  {
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_NONE;
    RCC_OscInitStruct.PLL1.PLLState  = RCC_PLL_NONE;
    RCC_OscInitStruct.PLL2.PLLState  = RCC_PLL_NONE;
    RCC_OscInitStruct.PLL3.PLLState  = RCC_PLL_NONE;
    RCC_OscInitStruct.PLL4.PLLState  = RCC_PLL_ON;              /* nur PLL4 = 1800 MHz */
    RCC_OscInitStruct.PLL4.PLLSource = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL4.PLLM = 8;    /* 64 MHz / 8 = 8 MHz PLL-Eingang */
    RCC_OscInitStruct.PLL4.PLLN = 225;  /* 8 MHz * 225 = 1800 MHz VCO     */
    RCC_OscInitStruct.PLL4.PLLFractional = 0;
    RCC_OscInitStruct.PLL4.PLLP1 = 1;
    RCC_OscInitStruct.PLL4.PLLP2 = 1;   /* 1800 / (1*1) = 1800 MHz        */
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
      Error_Handler();
    }
  }
#endif

  /* --- CPU-Domäne (sysa_ck / IC1 aus PLL2), Teiler aus inference_conf.h --- */
  RCC_ClkInitStruct.ClockType    = RCC_CLOCKTYPE_CPUCLK;
  RCC_ClkInitStruct.CPUCLKSource = RCC_CPUCLKSOURCE_IC1;
  RCC_ClkInitStruct.IC1Selection.ClockSelection = RCC_ICCLKSOURCE_PLL2;
  RCC_ClkInitStruct.IC1Selection.ClockDivider   = IC1_CLOCK_DIVIDER;

  /* --- CPURAM/NPU/NPURAM (sysb/sysc/sysd = IC2/IC6/IC11) ---
   * Der SYSCLK-Trio wird nur (re-)konfiguriert, wenn alle drei Teiler definiert
   * sind, sonst bleiben sysb/sysc/sysd auf den von der FSBL gesetzten Bootwerten.
   * IC2 kommt aus PLL1 (2400 MHz); IC6 (NPU) aus PLL1 oder PLL3 (2000 -> 1000 MHz);
   * IC11 (NPURAM) aus PLL1 oder PLL4 (1800 -> 900 MHz). PLL3/PLL4 oben zugeschaltet.
   * HINWEIS: IC2 (sysb/AXI) ist der Bus, ueber den die CPU XIP-Instruktionen holt.
   *          Weicht IC2_CLOCK_DIVIDER vom FSBL-Bootwert ab, wird dieser Bus live
   *          umgeteilt (glitchlos; Flash-Timing haengt an IC3 -> unkritisch, aber
   *          pro Config testen). */
#if defined(IC2_CLOCK_DIVIDER) && defined(IC6_CLOCK_DIVIDER) && defined(IC11_CLOCK_DIVIDER)
  RCC_ClkInitStruct.ClockType   |= RCC_CLOCKTYPE_SYSCLK;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_IC2_IC6_IC11;
  RCC_ClkInitStruct.IC2Selection.ClockSelection  = RCC_ICCLKSOURCE_PLL1; /* CPURAM/sysb */
  RCC_ClkInitStruct.IC2Selection.ClockDivider    = IC2_CLOCK_DIVIDER;
  RCC_ClkInitStruct.IC6Selection.ClockSelection  = IC6_CLOCK_SOURCE;      /* NPU/sysc: PLL1 (2400) oder PLL3 (2000, fuer 1000 MHz) */
  RCC_ClkInitStruct.IC6Selection.ClockDivider    = IC6_CLOCK_DIVIDER;
  RCC_ClkInitStruct.IC11Selection.ClockSelection = IC11_CLOCK_SOURCE;     /* NPURAM/sysd: PLL1 (2400) oder PLL4 (1800, fuer 900 MHz) */
  RCC_ClkInitStruct.IC11Selection.ClockDivider   = IC11_CLOCK_DIVIDER;
#endif

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
