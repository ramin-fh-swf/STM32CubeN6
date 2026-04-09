#ifndef __RM_INIT
#define __RM_INIT

#ifdef __cplusplus
extern "C"
{
#endif

#include "main.h"

extern UART_HandleTypeDef hlpuart1;

void rm_init_calculate_print_freqs(void);
void rm_init_uart(void);

#ifdef __cplusplus
}
#endif

#endif
