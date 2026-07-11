/*
 * rm_ai_lenet5.h
 *
 *  Created on: xx.xx.xxxx
 *      Author: rama
 */

#ifndef RM_INC_RM_AI_EFFICIENTNETV2B3_300_H_
#define RM_INC_RM_AI_EFFICIENTNETV2B3_300_H_

/* From network_analyze_report.txt (Memory usage information) */
#define MODEL_WEIGHTS_SIZE_MB       (16.386f)
#define MODEL_ACTIVATIONS_SIZE_MB   (1.894f)   /* (inkl. I/O-Buffer in activations) */
#define MODEL_MEMORY_USAGE_TOTAL_MB (18.280f)

#define MODEL_WEIGHTS_SIZE_KB       (float)(MODEL_WEIGHTS_SIZE_MB * 1024U)
#define MODEL_ACTIVATIONS_SIZE_KB   (float)(MODEL_ACTIVATIONS_SIZE_MB *1024U)
#define MODEL_MEMORY_USAGE_TOTAL_KB (float)(MODEL_MEMORY_USAGE_TOTAL_MB *1024U)

#define MODEL_WEIGHTS_PLACEMENT     "External Flash"
#define MODEL_ACTIVATIONS_PLACEMENT "SRAM (CPU-RAM: AXISRAM2; NPU-RAM: AXISRAM3, AXISRAM4 and AXISRAM5)"

void rm_ai_efficientnetv2b3_300_preprocess(const NN_Instance_TypeDef *nn_instance);
void rm_ai_efficientnetv2b3_300_postprocess(const NN_Instance_TypeDef *nn_instance);

#endif /* RM_INC_RM_AI_EFFICIENTNETV2B3_300_H_ */
