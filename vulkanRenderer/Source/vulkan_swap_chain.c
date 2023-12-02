//
//  vulkan_swapchain.c
//  vulkanRenderer
//
//  Created by Markus Höglin on 2023-11-28.
//

#include "vulkan_swap_chain.h"

void query_surface_present_modes(VkPhysicalDevice device, VkSurfaceKHR surface, dynamic_vector *present_modes) {
    VkSurfaceCapabilitiesKHR surface_capabilities;
    uint32_t format_count = 0;
    uint32_t present_mode_count = 0;

    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &present_mode_count, NULL);

    VkPresentModeKHR present_mode_array[present_mode_count];

    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &present_mode_count, present_mode_array);

    for(uint32_t i = 0; i < present_mode_count; i++) {
        vector_add(present_modes, present_mode_array + i);
    }
}

void query_surface_formats(VkPhysicalDevice device, VkSurfaceKHR surface, dynamic_vector *surface_formats) {
    VkSurfaceCapabilitiesKHR surface_capabilities;
    uint32_t format_count = 0;
    uint32_t present_mode_count = 0;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &surface_capabilities);
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &format_count, NULL);
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &present_mode_count, NULL);

    VkSurfaceFormatKHR formats[format_count];
    VkPresentModeKHR modes[present_mode_count];

    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &format_count, formats);
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &present_mode_count, modes);

    for(uint32_t i = 0; i < present_mode_count; i++) {
        vector_add(surface_formats, formats + i);
    }

}

VkSurfaceFormatKHR choose_swap_surface_format(VkPhysicalDevice device, VkSurfaceKHR surface) {
    uint32_t format_count = 0;
    VkSurfaceFormatKHR format = {0};
    return format;
}

void create_swap_chain(VkSwapchainKHR *swap_chain, VkPhysicalDevice physical_device, VkSurfaceKHR surface, uint32_t image_count, uint32_t swap_chain_usage, VkSurfaceFormatKHR surface_format, VkPresentModeKHR present_mode, VkExtent2D image_extent) {
    VkSurfaceCapabilitiesKHR capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, surface, &capabilities);
    
    VkSwapchainCreateInfoKHR create_info = {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .surface = surface,
        .imageUsage = swap_chain_usage,//Specifies what kinds of operations the images will be used for
        .minImageCount = image_count,
        .imageFormat = surface_format.format,
        .imageColorSpace = surface_format.colorSpace,
        .imageExtent = image_extent,
        .imageArrayLayers = 1,
        .presentMode = present_mode,
        .preTransform = capabilities.currentTransform,//Can specify transforms to apply to images in swap chain
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,//Opaque -> alpha channel should not be used to blend with other windows
        .clipped = VK_TRUE,//True -> we don't care about color of obscured pixel
        .oldSwapchain = VK_NULL_HANDLE//If swap chain must be replaced, then reference to old one must be given here
    };


    

    return;
}