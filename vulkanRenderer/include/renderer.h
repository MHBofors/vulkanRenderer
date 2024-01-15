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
    VkPhysicalDevice physical_device;
    VkDevice logical_device;
    device_queues queues;
    VkSwapchainKHR swap_chain;
} vulkan_context_t;

void setup_context(vulkan_context_t *context, VkInstance instance, VkSurfaceKHR surface, VkExtent2D extent);




#endif /* renderer_h */
