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

VkPresentModeKHR choose_swap_present_mode(VkPhysicalDevice physical_device, VkSurfaceKHR surface);

VkSurfaceFormatKHR choose_swap_surface_format(VkPhysicalDevice physical_device, VkSurfaceKHR surface);

void create_swap_chain(VkSwapchainKHR *swap_chain, VkDevice device, VkPhysicalDevice physical_device, VkSurfaceKHR surface, uint32_t image_count, uint32_t image_usage, VkSurfaceFormatKHR surface_format, VkPresentModeKHR present_mode, VkExtent2D image_extent);

void get_swap_chain_images(VkSwapchainKHR swap_chain, VkDevice device, dynamic_vector *swap_chain_images);
#endif