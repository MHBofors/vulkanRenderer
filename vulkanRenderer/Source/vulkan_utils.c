//
//  vulkan_utils.c
//  vulkanRenderer
//
//  Created by Markus Höglin on 2023-10-27.
//

#include "vulkan_utils.h"
#include "vulkan_debug.h"


uint32_t check_extension_support(const char **p_available, uint32_t available_count, const char **p_required, uint32_t required_count) {
    uint32_t unsupported_extension_count = 0;
    
    for(int i = 0; i < required_count; i++)
    {
        int check = 0;
        for(int j = 0; j < available_count; j++)
        {
            if(strcmp(p_required[i], p_available[j]))
            {
                check = 1;
                break;
            }
        }
        
        if(check)
        {
            printf("\t%s - supported\n", p_required[i]);
        }
        else
        {
            unsupported_extension_count++;
            printf("\t%s - not supported\n", p_required[i]);
        }
    }
    return unsupported_extension_count;
}

