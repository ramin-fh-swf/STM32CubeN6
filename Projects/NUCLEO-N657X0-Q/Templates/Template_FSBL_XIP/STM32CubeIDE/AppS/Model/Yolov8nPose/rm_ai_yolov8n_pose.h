#ifndef RM_INC_RM_AI_YOLOV8N_POSE_H_
#define RM_INC_RM_AI_YOLOV8N_POSE_H_

/* From network_analyze_report.txt (Memory usage information) */
#define MODEL_WEIGHTS_SIZE_MB       (3.157f)
#define MODEL_ACTIVATIONS_SIZE_MB   (0.801f)   /* (inkl. I/O-Buffer in activations) */
#define MODEL_MEMORY_USAGE_TOTAL_MB (3.958f)

#define MODEL_WEIGHTS_SIZE_KB       (float)(MODEL_WEIGHTS_SIZE_MB * 1024U)
#define MODEL_ACTIVATIONS_SIZE_KB   (float)(MODEL_ACTIVATIONS_SIZE_MB *1024U)
#define MODEL_MEMORY_USAGE_TOTAL_KB (float)(MODEL_MEMORY_USAGE_TOTAL_MB *1024U)

#define MODEL_WEIGHTS_PLACEMENT     "External Flash"
#define MODEL_ACTIVATIONS_PLACEMENT "SRAM (CPU-RAM: -; NPU-RAM: AXISRAM4 and AXISRAM5)"

void rm_ai_yolov8n_pose_preprocess(const NN_Instance_TypeDef *nn_instance);
void rm_ai_yolov8n_pose_postprocess(const NN_Instance_TypeDef *nn_instance);

#endif /* RM_INC_RM_AI_YOLOV8N_POSE_H_ */
