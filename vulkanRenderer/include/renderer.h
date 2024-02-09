//
//  renderer.h
//  vulkanRenderer
//
//  Created by Markus Höglin on 2023-10-27.
//
#ifndef renderer_h
#define renderer_h

#include <stdio.h>
#include "vulkan_instance.h"
#include "vulkan_device.h"
#include "vulkan_swap_chain.h"
#include "vulkan_command_buffers.h"

typedef struct vulkan_context {
    VkInstance instance;
    VkSurfaceKHR surface;
    VkDebugUtilsMessengerEXT debug_messenger;
} vulkan_context_t;

typedef struct device_context {
    VkPhysicalDevice physical_device;
    VkDevice logical_device;
    device_queues queues;
} device_context_t;

typedef struct swap_resources_t {
    VkSwapchainKHR swap_chain;
    VkExtent2D extent;
    VkFormat image_format;
    uint32_t image_count;
    VkImage *images;
    VkImageView *image_views;
} swap_resources_t;

typedef struct shader_details_t {
    uint32_t num_stages;
    VkPipelineShaderStageCreateInfo *shader_stages;
    VkPipelineVertexInputStateCreateInfo vertex_input;
} shader_details_t;

typedef struct pipeline_details_t {
    uint32_t stage_count;
    VkPipelineShaderStageCreateInfo *shader_stages;
    VkPipelineVertexInputStateCreateInfo vertex_input;
    VkPipelineInputAssemblyStateCreateInfo input_assembly;
    VkPipelineViewportStateCreateInfo viewport;
    VkPipelineRasterizationStateCreateInfo rasterizer;
    VkPipelineMultisampleStateCreateInfo multisampler;
    VkPipelineDepthStencilStateCreateInfo depth_stencil;
    VkPipelineColorBlendStateCreateInfo color_blender;
    VkPipelineDynamicStateCreateInfo dynamic_state;
    VkPipelineLayoutCreateInfo layout;
} pipeline_details_t;

typedef struct render_pipeline_t {
    VkRenderPass render_pass;
    VkPipelineLayout pipeline_layout;
    VkPipeline graphics_pipeline;
} render_pipeline_t;

typedef struct buffer_t {
    VkBuffer buffer;
    VkDeviceMemory memory;
} buffer_t;

typedef struct frame_t {
    VkBuffer buffer;
    VkDeviceMemory memory;
    void *mapped_memory;
} host_buffer_t;

typedef struct {
    VkFence in_flight_fence;
    VkSemaphore image_available_semaphore;
    VkSemaphore render_finished_semaphore;
    VkCommandBuffer command_buffer;
} frame_t;

typedef struct renderer_t {
    vulkan_context_t context;
    device_context_t device_context;
    VkSwapchainKHR swap_chain;
    VkCommandPool command_pool;
    uint32_t frame_count;
    frame_t *frames;
} renderer_t;

void setup_device_context(device_context_t *device_context, vulkan_context_t *context);

void create_render_pass(VkRenderPass *render_pass, uint32_t attachment_count, VkAttachmentDescription *attachment_descriptions, VkSubpassDescription *sub_pass);

void create_render_pass_simple(VkRenderPass *render_pass, VkDevice logical_device, VkFormat image_format);

void clear_pipeline_details(pipeline_details_t *pipeline_details);

void set_input_topology(pipeline_details_t *pipeline_details, VkPrimitiveTopology topology);

void set_polygon_mode(pipeline_details_t *pipeline_details, VkPolygonMode polygon_mode);

void set_cull_mode(pipeline_details_t *pipeline_details, VkCullModeFlags cull_mode, VkFrontFace front_face);

void set_multisampler_none(pipeline_details_t *pipeline_details);

void set_color_blending_none(pipeline_details_t *pipeline_details, VkPipelineColorBlendAttachmentState *color_blend_attachment);

void set_blend_attachment_none(VkPipelineColorBlendAttachmentState *color_blend_attachment);

void set_depth_test_none(pipeline_details_t *pipeline_details);

void create_graphics_pipeline(VkPipeline *graphics_pipeline, VkPipelineLayout *pipeline_layout, VkDevice logical_device, VkRenderPass render_pass, pipeline_details_t *pipeline_details);

void create_vertex_buffer(buffer_t *vertex_buffer, dynamic_vector *vertex_vector);

void create_index_buffer(buffer_t *index_buffer, dynamic_vector *index_vector);

void create_frame(frame_t *frame, VkDevice logical_device, VkCommandBuffer *command_buffer);

void begin_frame(frame_t *frame);

void end_frame();

#endif /* renderer_h */
