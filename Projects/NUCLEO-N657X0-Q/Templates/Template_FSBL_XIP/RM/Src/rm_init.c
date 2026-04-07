#include "rm_init.h"
#include "main.h"
#include "stdio.h"
#include <errno.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>

#define RM_INIT_DESIRED_CPU_FREQ 800000000u

void rm_init_calculate_print_freqs(void)
{
	/*
    printf("CPU Clock Frequency: %lu Hz\n", HAL_RCC_GetCpuClockFreq());
    printf("System Clock Frequency: %lu Hz\n", HAL_RCC_GetSysClockFreq());
    printf("NPU Clock Frequency: %lu Hz\n", HAL_RCC_GetNPUClockFreq());
    printf("NPU RAMS Clock Frequency: %lu Hz\n", HAL_RCC_GetNPURAMSClockFreq());
	*/

	uint32_t freq = HAL_RCC_GetCpuClockFreq();
	freq = HAL_RCC_GetSysClockFreq();
	freq = HAL_RCC_GetNPUClockFreq();
	freq = HAL_RCC_GetNPURAMSClockFreq();
	(void)freq;

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

/* Stub implementations ***************************************************************/
