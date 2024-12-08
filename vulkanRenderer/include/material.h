#ifndef material_h
#define material_h

#include <stdlib.h>
#include <stdio.h>
#include <vulkan/vulkan.h>

typedef struct material_pipeline_t {
    VkPipeline pipeline;
    VkPipelineLayout pipeline_layout;
} material_pipeline_t;

typedef struct material_t {
    material_pipeline_t *material_pipeline;
    VkDescriptorSet descriptor;
} material_t;

typedef struct render_object_t {
    uint32_t index_count;
    uint32_t first_index;
    VkBuffer index_buffer;
    VkBuffer *vertex_buffer;
    material_t *material_instance;
} render_object_t;

#endif