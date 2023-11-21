//
//  vulkan_device.h
//  vulkanRenderer
//
//  Created by Markus Höglin on 2023-11-06.
//

#ifndef vulkan_device_h
#define vulkan_device_h

#include <stdio.h>

#endif /* vulkan_device_h */


#include <stdlib.h>
#include <vulkan/vulkan.h>
#include "vulkan_utils.h"

typedef struct {
    uint32_t flag_bits;
    uint32_t graphics_family;
    uint32_t present_family;
} queue_family_indices;

void select_physical_device(VkPhysicalDevice *physical_device, VkInstance instance, VkSurfaceKHR surface);

uint32_t check_device_suitability(VkPhysicalDevice device, VkSurfaceKHR surface);

queue_family_indices find_queue_families(VkPhysicalDevice device, VkSurfaceKHR surface);

uint32_t is_complete(queue_family_indices indices);

