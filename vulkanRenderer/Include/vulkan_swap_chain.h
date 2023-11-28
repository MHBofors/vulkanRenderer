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
#include "vulkan_utils.h"

void query_swap_chain_support(VkPhysicalDevice device, VkSurfaceKHR surface, dynamic_vector *surface_formats, dynamic_vector *present_modes);

#endif