//
//  renderer.c
//  vulkanRenderer
//
//  Created by Markus Höglin on 2023-10-27.
//

#include "renderer.h"

void setup_context(vulkan_context_t *context, VkInstance instance, VkSurfaceKHR surface, VkExtent2D extent) {
    dynamic_vector *device_config = vector_alloc(sizeof(const char *));
    select_physical_device(&context->physical_device, instance, surface);
    create_logical_device(&context->logical_device, context->physical_device, surface, &context->queues, device_config);

    VkPresentModeKHR present_mode = choose_swap_present_mode(context->physical_device, surface);
    VkSurfaceFormatKHR surface_format = choose_swap_surface_format(context->physical_device, surface);
    VkSurfaceCapabilitiesKHR capabilities; 

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(context->physical_device, surface, &capabilities);

    create_swap_chain(&context->swap_chain, context->logical_device, context->physical_device, surface, capabilities.minImageCount + 1, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, surface_format, present_mode, extent);
}
