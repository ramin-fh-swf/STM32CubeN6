/*
 * rm_ai.c
 *
 *  Created on: 18.05.2026
 *      Author: rama
 */

#include "rm_ai.h"

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <string.h>

#include "app_x-cube-ai.h"
#include "bsp_ai.h"
#include "stai.h"
#include "npu_init.h"

#ifdef MODEL_YAMNET
#include "yamnet_esc10_patches.h"
#endif

#ifdef MODEL_YAMNET
#include "yamnet_esc10_patches.h"
#endif

