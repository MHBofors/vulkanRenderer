//
//  vulkan_resources.h
//  vulkanRenderer
//
//  Created by Markus Höglin on 2023-11-06.
//

#ifndef vulkan_resources_h
#define vulkan_resources_h

#include <stdio.h>
#include <vulkan/vulkan.h>
#include "vulkan_utils.h"

void create_image();

void create_image_view();

void create_buffer(VkBuffer buffer, VkDeviceMemory buffer_memory, VkDevice device, VkDeviceSize device_size, VkBufferUsageFlagBits buffer_usage, VkMemoryPropertyFlags properties);

#endif /* vulkan_resources_h */
