/*
 * rm_ai_lenet5.h
 *
 *  Created on: 19.05.2026
 *      Author: rama
 */

#ifndef RM_INC_RM_AI_YAMNET_H_
#define RM_INC_RM_AI_YAMNET_H_

/* From network_analyze_report.txt (Memory usage information) */
#define MODEL_WEIGHTS_SIZE_KB       (167.657f)   /* 167.657 kB */
#define MODEL_ACTIVATIONS_SIZE_KB   (144.000f)   /* 144.000 kB (inkl. I/O-Buffer) */
#define MODEL_MEMORY_USAGE_TOTAL_KB (311.657f)

#define MODEL_WEIGHTS_PLACEMENT     "External Flash"
#define MODEL_ACTIVATIONS_PLACEMENT "SRAM (NPU-RAM: AXISRAM5)"

void rm_ai_yamnet_preprocess(const NN_Instance_TypeDef *nn_instance);
void rm_ai_yamnet_postprocess(const NN_Instance_TypeDef *nn_instance);

#endif /* RM_INC_RM_AI_YAMNET_H_ */
