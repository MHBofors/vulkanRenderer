//
//  vulkan_utils.h
//  vulkanRenderer
//
//  Created by Markus Höglin on 2023-10-27.
//

#ifndef vulkan_utils_h
#define vulkan_utils_h

#include <stdio.h>
#include <stdlib.h>
#include "dynamic_vector.h"

uint32_t check_extension_support(const char **p_available, uint32_t available_count, const char **p_required, uint32_t required_count);
#endif /* vulkan_utils_h */
