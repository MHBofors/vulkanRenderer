#ifndef renderer_h
#define renderer_h

#include "window.h"
#include "vulkan_render_pipeline.h"
#include "vulkan_instance.h"
#include "vulkan_device.h"
#include "vulkan_swap_chain.h"
#include "vulkan_command_buffers.h"

typedef struct {
    float x;
    float y;
    float z;
} vector_t;

typedef struct {
    float r;
    float g;
    float b;
    float alpha;
} color_t;

typedef struct {
    vector_t position;
    color_t color;
} vertex_t;

typedef struct vulkan_context_t {
    VkInstance instance;
    VkSurfaceKHR surface;
    VkDebugUtilsMessengerEXT debug_messenger;
} vulkan_context_t;

typedef struct device_context_t {
    VkPhysicalDevice physical_device;
    VkDevice logical_device;
    device_queues queues;
} device_context_t;

typedef struct swap_chain_t {
    VkSwapchainKHR swap_chain;
    VkExtent2D extent;
    VkFormat image_format;
    uint32_t image_count;
    VkImage *images;
    VkImageView *image_views;
    VkFramebuffer *framebuffers;
} swap_resources_t;

typedef struct render_pipeline_t {
    VkRenderPass render_pass;
    VkPipelineLayout pipeline_layout;
    VkPipeline graphics_pipeline;
} render_pipeline_t;

typedef struct buffer_t {
    VkBuffer buffer;
    VkDeviceMemory memory;
} buffer_t;

typedef struct host_buffer_t {
    VkBuffer buffer;
    VkDeviceMemory memory;
    void *mapped_memory;
} host_buffer_t;

typedef struct frame_t {
    VkFence in_flight_fence;
    VkSemaphore image_available_semaphore;
    VkSemaphore render_finished_semaphore;
    VkCommandBuffer command_buffer;
} frame_t;

typedef struct renderer_t {
    window_t window;
    vulkan_context_t vulkan_context;
    device_context_t device_context;
    swap_resources_t swap_resources;
    render_pipeline_t render_pipeline;
    VkCommandPool command_pool;
    uint32_t frame_count;
    frame_t *frames;
} renderer_t;

void setup_context(vulkan_context_t *context, window_t window);

void setup_device_context(device_context_t *device_context, vulkan_context_t *context);

void setup_swap_resources(swap_resources_t *swap_resources, vulkan_context_t *vulkan_context, device_context_t *device_context, window_t window);

void setup_render_pipeline_simple(render_pipeline_t *render_pipeline, device_context_t device_context, swap_resources_t swap_resources);

void clean_up_context(vulkan_context_t *context);

void clean_up_device_context(device_context_t *device_context);

void clean_up_swap_resources(swap_resources_t *swap_resources, device_context_t *device_context);

void clean_up_render_pipeline(render_pipeline_t *render_pipeline, device_context_t *device_context);

void create_vertex_buffer(buffer_t *vertex_buffer, dynamic_vector *vertex_vector, VkDevice logical_device, VkPhysicalDevice physical_device, VkQueue queue, VkCommandPool command_pool);

void create_index_buffer(buffer_t *index_buffer, dynamic_vector *index_vector, VkDevice logical_device, VkPhysicalDevice physical_device, VkQueue queue, VkCommandPool command_pool);

void destroy_buffer(buffer_t *buffer, VkDevice logical_device);

void create_frame(frame_t *frame, VkDevice logical_device, VkCommandPool command_pool);

frame_t *create_frames(VkDevice logical_device, VkCommandPool command_pool, uint32_t frame_count);

void clean_up_frame(frame_t *frames, VkDevice logical_device);

void clean_up_frames(frame_t *frames, uint32_t frame_count, VkDevice logical_device);

void draw_frame();

uint32_t begin_frame(frame_t *frame, VkDevice logical_device, VkSwapchainKHR swap_chain);

void end_frame(frame_t *frame, VkSwapchainKHR swap_chain, VkQueue graphics_queue, VkQueue present_queue, uint32_t image_index);

#endif /* renderer_h */