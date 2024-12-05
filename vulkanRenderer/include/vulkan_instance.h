//
//  vulkan_instance.h
//  vulkanRenderer
//
//  Created by Markus Höglin on 2023-11-16.
//

#ifndef vulkan_instance_h
#define vulkan_instance_h

#include <stdio.h>
#include <vulkan/vulkan.h>
#include "vulkan_utils.h"
#include "vulkan_debug.h"

void apple_instance_support(VkInstanceCreateInfo *create_info, uint32_t extension_count, const char *extensions[]);

void create_instance(VkInstance *instance, uint32_t extension_count, const char *extensions[]);

#endif /* vulkan_instance_h */