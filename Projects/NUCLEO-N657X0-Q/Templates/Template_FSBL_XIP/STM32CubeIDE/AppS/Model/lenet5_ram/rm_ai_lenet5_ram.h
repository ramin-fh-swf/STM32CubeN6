/*
 * rm_ai_lenet5.h
 *
 *  Created on: 19.05.2026
 *      Author: rama
 */

#ifndef RM_INC_RM_AI_LENET5_RAM_H_
#define RM_INC_RM_AI_LENET5_RAM_H_

/* From network_analyze_report.txt (Memory usage information) */
#define MODEL_WEIGHTS_SIZE_KB       (43.220f)
#define MODEL_ACTIVATIONS_SIZE_KB   (1.609f)   /* (inkl. I/O-Buffer) */
#define MODEL_MEMORY_USAGE_TOTAL_KB (44.829f)

#define MODEL_WEIGHTS_PLACEMENT     "SRAM (NPU-RAM: AXISRAM3)"
#define MODEL_ACTIVATIONS_PLACEMENT "SRAM (NPU-RAM: AXISRAM3)"

void rm_ai_lenet5_ram_setup_sram(void);
void rm_ai_lenet5_ram_preprocess(const NN_Instance_TypeDef *nn_instance);
void rm_ai_lenet5_ram_postprocess(const NN_Instance_TypeDef *nn_instance);

#endif /* RM_INC_RM_AI_LENET5_RAM_H_ */
