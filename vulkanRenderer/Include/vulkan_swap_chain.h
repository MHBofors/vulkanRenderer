//
//  vulkan_swapchain.h
//  vulkanRenderer
//
//  Created by Markus Höglin on 2023-11-28.
//

#ifndef vulkan_swap_chain_h
#define vulkan_swap_chain_h

#include <stdio.h>
#include <vulkan/vulkan.h>
#include "vulkan_device.h"
#include "vulkan_utils.h"

void query_swap_chain_support(VkPhysicalDevice device, VkSurfaceKHR surface, dynamic_vector *surface_formats, dynamic_vector *present_modes);

void create_swap_chain(VkSwapchainKHR *swap_chain, VkPhysicalDevice physical_device, VkSurfaceKHR surface, uint32_t image_count, uint32_t swap_chain_usage, VkSurfaceFormatKHR surface_format, VkPresentModeKHR present_mode, VkExtent2D image_extent);

#endif