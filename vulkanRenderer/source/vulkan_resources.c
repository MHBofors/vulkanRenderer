//
//  vulkan_resources.c
//  vulkanRenderer
//
//  Created by Markus Höglin on 2023-11-06.
//

#include "vulkan_resources.h"

uint32_t select_memory_type(VkPhysicalDevice physical_device, uint32_t type_filter, VkMemoryPropertyFlags properties) {
    VkPhysicalDeviceMemoryProperties device_memory_properties;
    vkGetPhysicalDeviceMemoryProperties(physical_device, &device_memory_properties);

    for(uint32_t i = 0; i < device_memory_properties.memoryTypeCount; i++) {
        if(type_filter & (1 << i) && device_memory_properties.memoryTypes[i].propertyFlags & properties) {
            return i;
        }
    }

    error(1, "Failed to find suitable memory type\n");
}

void create_image() {

}

void create_image_view(VkImage *image, VkImageView *image_view, VkFormat image_format) {

}

void create_buffer(VkBuffer *buffer, VkDeviceMemory *buffer_memory, VkDevice logical_device, VkPhysicalDevice physical_device, VkDeviceSize device_size, VkBufferUsageFlagBits buffer_usage, VkMemoryPropertyFlags properties) {
    VkBufferCreateInfo buffer_create_info = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = device_size,
        .usage = buffer_usage,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE
    };

    if(vkCreateBuffer(logical_device, &buffer_create_info, NULL, buffer) != VK_SUCCESS) {
        error(1, "Failed to create buffer\n");
    }

    VkMemoryRequirements memory_requirements;
    vkGetBufferMemoryRequirements(logical_device, *buffer, &memory_requirements);

    VkMemoryAllocateInfo allocate_info = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .allocationSize = memory_requirements.size,
        .memoryTypeIndex = select_memory_type(physical_device, memory_requirements.memoryTypeBits, properties)
    };

    if(vkAllocateMemory(logical_device, &allocate_info, NULL, buffer_memory) != VK_SUCCESS) {
        error(1, "Failed to allocate buffer memory\n");
    }

    vkBindBufferMemory(logical_device, *buffer, *buffer_memory, 0);
}