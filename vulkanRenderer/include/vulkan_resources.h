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

uint32_t select_memory_type(VkPhysicalDevice physical_device, uint32_t type_filter, VkMemoryPropertyFlags properties);

void create_image(VkImage *image, VkDeviceMemory *image_memory, VkDevice logical_device, VkPhysicalDevice physical_device, uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties);
void create_image_view(VkImageView *image_view, VkImage image, VkDevice logical_device, VkFormat image_format);

void create_buffer(VkBuffer *buffer, VkDeviceMemory *buffer_memory, VkDevice logical_device, VkPhysicalDevice physical_device, VkDeviceSize device_size, VkBufferUsageFlagBits buffer_usage, VkMemoryPropertyFlags properties);
void copy_buffer(VkBuffer dest_buffer, VkBuffer source_buffer, VkDevice logical_device, VkCommandPool command_pool, VkQueue queue, VkDeviceSize size);

#endif /* vulkan_resources_h */
