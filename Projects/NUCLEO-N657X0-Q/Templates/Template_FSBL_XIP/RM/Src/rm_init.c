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
    /* SCK = XSPI2 Clock / (DCR2.PRESCALER + 1). XSPI2 Clock: (IC3 <- PLL1),*/
    {
      uint32_t xspi2_ker_hz = HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_XSPI2);
      uint32_t prescaler    = (XSPI2->DCR2 & XSPI_DCR2_PRESCALER_Msk) >> XSPI_DCR2_PRESCALER_Pos;
      uint32_t sck_hz       = xspi2_ker_hz / (prescaler + 1U);
      printf(" - XSPI2 (Flash) SCK:        %lu Hz (%lu.%02lu MHz) [kernel=%lu Hz, DCR2.PRESCALER=%lu]\n",
             (unsigned long)sck_hz,
             (unsigned long)(sck_hz / 1000000U),
             (unsigned long)((sck_hz % 1000000U) / 10000U),
             (unsigned long)xspi2_ker_hz,
             (unsigned long)prescaler);

      /* Lesekommando der Speicher-Mapped-Konfiguration. Bestimmt zusammen mit SCK
       * die Spitzenbandbreite, mit der die NPU Gewichte aus dem XIP-Flash streamt:
       *   Peak = SCK * Datenleitungen * (DTR ? 2 : 1) / 8 Byte/s
       * DMODE-Kodierung laut CCR: 1 = 1, 2 = 2, 3 = 4, 4 = 8 Leitungen.
       * DCYC (Dummy-Zyklen) und die Adressphase gehen davon wieder ab, die
       * Spitzenbandbreite ist also eine obere Schranke, kein erreichbarer Wert. */
      {
        uint32_t ccr   = XSPI2->CCR;
        uint32_t dmode = (ccr & XSPI_CCR_DMODE_Msk) >> XSPI_CCR_DMODE_Pos;
        uint32_t ddtr  = (ccr & XSPI_CCR_DDTR_Msk) ? 1U : 0U;
        uint32_t dcyc  = (XSPI2->TCR & XSPI_TCR_DCYC_Msk) >> XSPI_TCR_DCYC_Pos;
        uint32_t fmode = (XSPI2->CR & XSPI_CR_FMODE_Msk) >> XSPI_CR_FMODE_Pos;
        uint32_t lines = (dmode == 1U) ? 1U : (dmode == 2U) ? 2U :
                         (dmode == 3U) ? 4U : (dmode == 4U) ? 8U : 0U;
        uint32_t peak_kBps = (sck_hz / 1000U) * lines * (ddtr ? 2U : 1U) / 8U;

        printf(" - XSPI2 (Flash) Read:       %lu lines %s, DCYC=%lu, peak=%lu.%03lu MB/s"
               " [CCR=0x%08lX, TCR=0x%08lX, CR.FMODE=%lu%s]\n",
               (unsigned long)lines,
               ddtr ? "DTR" : "SDR",
               (unsigned long)dcyc,
               (unsigned long)(peak_kBps / 1000U),
               (unsigned long)(peak_kBps % 1000U),
               (unsigned long)ccr,
               (unsigned long)XSPI2->TCR,
               (unsigned long)fmode,
               (fmode == 3U) ? " = memory-mapped" : " != memory-mapped");
      }
    }
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

