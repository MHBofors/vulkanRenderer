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

typedef struct {
    VkSwapchainKHR swap_chain;
    VkFormat image_format;
    VkExtent2D extent;
    VkImageView *image_views;
    VkImage *images;
    uint32_t image_count;
} swap_resource_t;

typedef struct {
    VkBuffer buffer;
    VkDeviceMemory memory;
} buffer_t;

typedef struct {
    VkBuffer buffer;
    VkDeviceMemory memory;
    void *mapped_memory;
} uniform_buffer_t;

typedef struct {
    VkFence in_flight_fence;
    VkSemaphore image_available_semaphore;
    VkSemaphore render_finished_semaphore;
    VkCommandBuffer command_buffer;
} frame_t;

typedef struct {
    vulkan_context_t context;
    device_context_t device_context;
    VkSwapchainKHR swap_chain;

    frame_t *frames;
} renderer_t;


void setup_context(vulkan_device_context_t *context, VkInstance instance, VkSurfaceKHR surface, VkExtent2D extent);

void create_render_pass(VkRenderPass *render_pass);

void create_graphics_pipeline();

void create_vertex_buffer(buffer_t *vertex_buffer, dynamic_vector *vertex_vector);

void create_index_buffer(buffer_t *index_buffer, dynamic_vector *index_vector);

#endif /* renderer_h */
