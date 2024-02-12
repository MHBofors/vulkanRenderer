

#include "renderer.h"

void setup_device_context(device_context_t *device_context, vulkan_context_t *context) {
    dynamic_vector *device_config = vector_alloc(sizeof(const char *));
    select_physical_device(&device_context->physical_device, context->instance, context->surface);
    create_logical_device(&device_context->logical_device, device_context->physical_device, context->surface, &device_context->queues, device_config);

    vector_free(device_config);
}

void setup_swap_resources(swap_resources_t *swap_resources, vulkan_context_t *vulkan_context, device_context_t *device_context, VkExtent2D extent) {
    VkPresentModeKHR present_mode = choose_swap_present_mode(device_context->physical_device, vulkan_context->surface);
    VkSurfaceFormatKHR surface_format = choose_swap_surface_format(device_context->physical_device, vulkan_context->surface);
    VkSurfaceCapabilitiesKHR capabilities;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device_context->physical_device, vulkan_context->surface, &capabilities);
    create_swap_chain(&swap_resources->swap_chain, device_context->logical_device, device_context->physical_device, vulkan_context->surface, capabilities.minImageCount + 1, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, surface_format, present_mode, extent);
}

void create_frame(frame_t *frame, VkDevice logical_device, VkCommandBuffer *command_buffer) {
    VkSemaphoreCreateInfo semaphore_create_info = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO
    };

    VkFenceCreateInfo fence_create_info = {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .flags = VK_FENCE_CREATE_SIGNALED_BIT
    };

    if(vkCreateSemaphore(logical_device, &semaphore_create_info, NULL, &frame->image_available_semaphore) != VK_SUCCESS |
       vkCreateSemaphore(logical_device, &semaphore_create_info, NULL, &frame->render_finished_semaphore) != VK_SUCCESS |
       vkCreateFence(logical_device, &fence_create_info, NULL, &frame->in_flight_fence) != VK_SUCCESS) {
        error(1, "Failed to create frame sync resources");
    }

    frame->command_buffer = command_buffer;
}

