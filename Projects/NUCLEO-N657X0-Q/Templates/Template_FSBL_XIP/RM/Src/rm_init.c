#include "rm_init.h"
#include "main.h"
#include "stdio.h"
#include <errno.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>

#define RM_INIT_DESIRED_CPU_FREQ 800000000u


UART_HandleTypeDef hlpuart1;

void MX_LPUART1_UART_Init(void)
{
  /* USER CODE BEGIN LPUART1_Init 0 */
  /* USER CODE END LPUART1_Init 0 */

  /* USER CODE BEGIN LPUART1_Init 1 */
  /* USER CODE END LPUART1_Init 1 */

  hlpuart1.Instance = LPUART1;
  hlpuart1.Init.BaudRate = 115200;
  hlpuart1.Init.WordLength = UART_WORDLENGTH_8B;
  hlpuart1.Init.StopBits = UART_STOPBITS_1;
  hlpuart1.Init.Parity = UART_PARITY_NONE;
  hlpuart1.Init.Mode = UART_MODE_TX_RX;
  hlpuart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  hlpuart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  hlpuart1.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  hlpuart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  hlpuart1.FifoMode = UART_FIFOMODE_DISABLE;
  if (HAL_UART_Init(&hlpuart1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&hlpuart1, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&hlpuart1, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&hlpuart1) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN LPUART1_Init 2 */
  /* USER CODE END LPUART1_Init 2 */
}

void rm_init_calculate_print_freqs(void)
{
    printf("Clock Frequencies ###\n");
    printf(" - CPU Clock Frequency:      %lu Hz\n", HAL_RCC_GetCpuClockFreq());
    printf(" - System Clock Frequency:   %lu Hz\n", HAL_RCC_GetSysClockFreq());
    printf(" - NPU Clock Frequency:      %lu Hz\n", HAL_RCC_GetNPUClockFreq());
    printf(" - NPU RAMS Clock Frequency: %lu Hz\n", HAL_RCC_GetNPURAMSClockFreq());
    printf(" - XSPI2 (Flash) SCK = %lu Hz\n", HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_XSPI2));
    printf(" - VCORE Voltage Scaling:    %s\n",
           (HAL_PWREx_GetVoltageRange() == PWR_REGULATOR_VOLTAGE_SCALE0) ? "SCALE0 (overdrive)" : "SCALE1 (nominal)");
    printf(" - VOSCR raw = 0x%08lX | VOSreq=%s VOSRDY=%lu ACTVOS=%s ACTVOSRDY=%lu\n",
           PWR->VOSCR,
           (PWR->VOSCR & PWR_VOSCR_VOS)       ? "SCALE0" : "SCALE1",
           (PWR->VOSCR & PWR_VOSCR_VOSRDY)    ? 1UL : 0UL,
           (PWR->VOSCR & PWR_VOSCR_ACTVOS)    ? "SCALE0" : "SCALE1",
           (PWR->VOSCR & PWR_VOSCR_ACTVOSRDY) ? 1UL : 0UL);

    /*
    uint32_t start_tick = HAL_GetTick();
    uint32_t start_cycles = DWT->CYCCNT;

    // wait ~1000 ms
    while ((HAL_GetTick() - start_tick) < 1000)
        ;
    uint32_t end_cycles = DWT->CYCCNT;
    uint32_t cycles = end_cycles - start_cycles;

    // frequency in Hz
    uint32_t cpu_freq = cycles; // since 1 has been measured
    printf("CPU Frequency: %lu\n", cpu_freq);
    */
}

void rm_init_uart(void)
{
	MX_LPUART1_UART_Init();
}


/* Stub implementations ***************************************************************/
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)

PUTCHAR_PROTOTYPE
{
    HAL_UART_Transmit(&hlpuart1, (uint8_t *)ch, 1, 0xFFFF);
    return ch;
}

__weak int _write(int fd, char *ptr, int len)
{
    HAL_UART_Transmit(&hlpuart1, (uint8_t *)ptr, len, HAL_MAX_DELAY);
    return len;
}

