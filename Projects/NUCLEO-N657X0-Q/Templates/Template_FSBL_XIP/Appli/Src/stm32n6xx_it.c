/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32n6xx_it.c
  * @brief   Interrupt Service Routines.
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
#include "stm32n6xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdint.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* Fault-Report (Debughilfe, vor Messlaeufen wieder entfernen)
 *
 * Gibt den gestackten Exception-Frame und die Fault-Status-Register aus.
 * Bewusst ohne printf: kein Heap, kein varargs, damit der Report auch bei
 * knappem Stack noch durchkommt.
 *
 * Annahme: Secure-only-Projekt. Der Frame liegt daher auf MSP_S bzw. PSP_S.
 */
extern int _write(int fd, char *ptr, int len);

static void fault_puts(const char *s)
{
  int len = 0;
  while (s[len] != '\0')
  {
    len++;
  }
  _write(1, (char *)s, len);
}

static void fault_hex(const char *label, uint32_t value)
{
  static const char digits[] = "0123456789ABCDEF";
  char buf[11];

  buf[0] = '0';
  buf[1] = 'x';
  for (int i = 0; i < 8; i++)
  {
    buf[2 + i] = digits[(value >> ((7 - i) * 4)) & 0xFU];
  }
  buf[10] = '\n';

  fault_puts(label);
  _write(1, buf, (int)sizeof(buf));
}

void rm_fault_report(uint32_t *frame, uint32_t exc_return)
{
  const uint32_t cfsr = SCB->CFSR;

  fault_puts("\n######## FAULT ########\n");
  fault_hex("EXC_RETURN ", exc_return);
  fault_hex("SP_frame   ", (uint32_t)frame);
  fault_hex("R0         ", frame[0]);
  fault_hex("R1         ", frame[1]);
  fault_hex("R2         ", frame[2]);
  fault_hex("R3         ", frame[3]);
  fault_hex("R12        ", frame[4]);
  fault_hex("LR         ", frame[5]);
  fault_hex("PC         ", frame[6]);
  fault_hex("xPSR       ", frame[7]);
  fault_hex("CFSR       ", cfsr);
  fault_hex("HFSR       ", SCB->HFSR);
  fault_hex("DFSR       ", SCB->DFSR);
  fault_hex("MMFAR      ", SCB->MMFAR); /* gueltig nur bei CFSR Bit 7  */
  fault_hex("BFAR       ", SCB->BFAR);  /* gueltig nur bei CFSR Bit 15 */
  fault_hex("AFSR       ", SCB->AFSR);
#if defined(__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
  fault_hex("SFSR       ", SAU->SFSR);  /* SFAR gueltig nur bei SFSR Bit 6 */
  fault_hex("SFAR       ", SAU->SFAR);
#endif
  fault_hex("SHCSR      ", SCB->SHCSR);
  fault_puts("#######################\n");

  while (1)
  {
  }
}

/* Ermittelt anhand EXC_RETURN Bit 2, auf welchem Stack der Frame liegt,
 * und uebergibt Frame-Zeiger und EXC_RETURN an den Report. */
#define RM_FAULT_HANDLER(name)                                                                                         \
  __attribute__((naked)) void name(void)                                                                               \
  {                                                                                                                    \
    __asm volatile("tst   lr, #4          \n"                                                                          \
                   "ite   eq              \n"                                                                          \
                   "mrseq r0, msp         \n"                                                                          \
                   "mrsne r0, psp         \n"                                                                          \
                   "mov   r1, lr          \n"                                                                          \
                   "b     rm_fault_report \n");                                                                        \
  }

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/

/* USER CODE BEGIN EV */

/* USER CODE END EV */

/******************************************************************************/
/*           Cortex Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */
   while (1)
  {
  }
  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
/* USER CODE BEGIN HardFault_IRQn 0 */
RM_FAULT_HANDLER(HardFault_Handler)
/* USER CODE END HardFault_IRQn 0 */

/**
  * @brief This function handles Memory management fault.
  */
/* USER CODE BEGIN MemoryManagement_IRQn 0 */
RM_FAULT_HANDLER(MemManage_Handler)
/* USER CODE END MemoryManagement_IRQn 0 */

/**
  * @brief This function handles Prefetch fault, memory access fault.
  */
/* USER CODE BEGIN BusFault_IRQn 0 */
RM_FAULT_HANDLER(BusFault_Handler)
/* USER CODE END BusFault_IRQn 0 */

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
/* USER CODE BEGIN UsageFault_IRQn 0 */
RM_FAULT_HANDLER(UsageFault_Handler)
/* USER CODE END UsageFault_IRQn 0 */

/**
  * @brief This function handles Secure fault.
  */
/* USER CODE BEGIN SecureFault_IRQn 0 */
RM_FAULT_HANDLER(SecureFault_Handler)
/* USER CODE END SecureFault_IRQn 0 */

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void)
{
  /* USER CODE BEGIN SVCall_IRQn 0 */

  /* USER CODE END SVCall_IRQn 0 */
  /* USER CODE BEGIN SVCall_IRQn 1 */

  /* USER CODE END SVCall_IRQn 1 */
}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */

  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void)
{
  /* USER CODE BEGIN PendSV_IRQn 0 */

  /* USER CODE END PendSV_IRQn 0 */
  /* USER CODE BEGIN PendSV_IRQn 1 */

  /* USER CODE END PendSV_IRQn 1 */
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
  /* USER CODE BEGIN SysTick_IRQn 1 */

  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32N6xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32n6xx.s).                    */
/******************************************************************************/

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
