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

uint32_t get_apple_extensions(dynamic_vector *vulkan_extension_config);

void create_instance(VkInstance *instance, dynamic_vector *vulkan_extension_config);

#endif /* vulkan_instance_h */