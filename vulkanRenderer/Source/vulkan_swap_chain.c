//
//  vulkan_swapchain.c
//  vulkanRenderer
//
//  Created by Markus Höglin on 2023-11-28.
//

#include "vulkan_swap_chain.h"

void query_swap_chain_support(VkPhysicalDevice device, VkSurfaceKHR surface, dynamic_vector *surface_formats, dynamic_vector *present_modes) {
    VkSurfaceCapabilitiesKHR surface_capabilities;
    uint32_t format_count = 0;
    uint32_t present_mode_count = 0;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &surface_capabilities);
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &format_count, NULL);
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &present_mode_count, NULL);

    VkSurfaceFormatKHR formats[format_count];
    VkPresentModeKHR modes[present_mode_count];

    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &format_count, formats);
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &present_mode_count, modes);

    for(uint32_t i = 0; i < present_mode_count; i++) {
        vector_add(surface_formats, formats + i);
    }

    for(uint32_t i = 0; i < present_mode_count; i++) {
        vector_add(present_modes, modes + i);
    }
}
