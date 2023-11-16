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

void create_instance(VkInstance *instance, dynamic_vector *vulkan_extension_config);

#endif /* vulkan_instance_h */