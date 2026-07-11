/*
 * rm_ai_lenet5.h
 *
 *  Created on: 19.05.2026
 *      Author: rama
 */

#ifndef RM_INC_RM_AI_LENET5_H_
#define RM_INC_RM_AI_LENET5_H_

/* From network_analyze_report.txt (Memory usage information) */
#define MODEL_WEIGHTS_SIZE_KB       (19.012f)
#define MODEL_ACTIVATIONS_SIZE_KB   (1.609f)   /* (inkl. I/O-Buffer) */
#define MODEL_MEMORY_USAGE_TOTAL_KB (20.621f)

#define MODEL_WEIGHTS_PLACEMENT     "External Flash"
#define MODEL_ACTIVATIONS_PLACEMENT "SRAM (NPU-RAM: AXISRAM5)"

void rm_ai_lenet5_preprocess(const NN_Instance_TypeDef *nn_instance);
void rm_ai_lenet5_postprocess(const NN_Instance_TypeDef *nn_instance);

#endif /* RM_INC_RM_AI_LENET5_H_ */
