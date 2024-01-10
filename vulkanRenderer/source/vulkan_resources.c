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

    return ~0;
}



void create_image(VkImage *image, VkDeviceMemory *image_memory, VkDevice logical_device, VkPhysicalDevice physical_device, uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties) {
    VkExtent3D image_extent = {
        .width = width,
        .height = height,
        .depth = 1
    };
    
    VkImageCreateInfo create_info = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .imageType = VK_IMAGE_TYPE_2D,
        .extent = image_extent,
        .mipLevels = 1,
        .arrayLayers = 1,
        .format = format,
        .tiling = tiling,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .usage = usage,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE
    };

    if(vkCreateImage(logical_device, &create_info, NULL, image) != VK_SUCCESS) {
        error(1, "Failed to create image\n");
    }

    VkMemoryRequirements memory_requirements;
    vkGetImageMemoryRequirements(logical_device, *image, &memory_requirements);

    uint32_t memory_type = select_memory_type(physical_device, memory_requirements.memoryTypeBits, properties);
    if(memory_type == ~0) {
        error(1, "Failed to find suitable memory type\n");
    }

    VkMemoryAllocateInfo alloc_info = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .allocationSize = memory_requirements.size,
        .memoryTypeIndex = memory_type
    };

    if(vkAllocateMemory(logical_device, &alloc_info, NULL, image_memory) != VK_SUCCESS) {
        error(1, "Failed to allocate image memory\n");
    }

    vkBindImageMemory(logical_device, *image, *image_memory, 0);
}

void create_image_view(VkImageView *image_view, VkImage image, VkDevice logical_device, VkFormat image_format) {
    VkImageSubresourceRange subresource_range = {
        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
        .baseMipLevel = 0,
        .levelCount = 1,
        .baseArrayLayer = 0,
        .layerCount = 1
    };
    
    VkImageViewCreateInfo create_info = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .image = image,
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format = image_format,
        .subresourceRange = subresource_range
    };

    vkCreateImageView(logical_device, &create_info, NULL, image_view);
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

    uint32_t memory_type = select_memory_type(physical_device, memory_requirements.memoryTypeBits, properties);
    if(memory_type == ~0) {
        error(1, "Failed to find suitable memory type\n");
    }

    VkMemoryAllocateInfo allocate_info = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .allocationSize = memory_requirements.size,
        .memoryTypeIndex = memory_type
    };

    if(vkAllocateMemory(logical_device, &allocate_info, NULL, buffer_memory) != VK_SUCCESS) {
        error(1, "Failed to allocate buffer memory\n");
    }

    vkBindBufferMemory(logical_device, *buffer, *buffer_memory, 0);
}

void copy_buffer(VkBuffer dest_buffer, VkBuffer source_buffer, VkDevice logical_device, VkCommandPool command_pool, VkQueue queue, VkDeviceSize size) {
    VkCommandBufferAllocateInfo alloc_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandPool = command_pool,
        .commandBufferCount = 1
    };

    VkCommandBuffer command_buffer;
    vkAllocateCommandBuffers(logical_device, &alloc_info, &command_buffer);

    VkCommandBufferBeginInfo begin_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT
    };

    vkBeginCommandBuffer(command_buffer, &begin_info);

    VkBufferCopy copy_region = {
        .dstOffset = 0,
        .srcOffset = 0,
        .size = size
    };

    vkCmdCopyBuffer(command_buffer, source_buffer, dest_buffer, 1, &copy_region);
    vkEndCommandBuffer(command_buffer);

    VkSubmitInfo submit_info = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .commandBufferCount = 1,
        .pCommandBuffers = &command_buffer
    };

    vkQueueSubmit(queue, 1, &submit_info, VK_NULL_HANDLE);
    vkQueueWaitIdle(queue);

    vkFreeCommandBuffers(logical_device, command_pool, 1, &command_buffer);
}
