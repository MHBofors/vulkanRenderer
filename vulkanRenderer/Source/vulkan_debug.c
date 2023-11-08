//
//  vulkan_debug.c
//  vulkanRenderer
//
//  Created by Markus Höglin on 2023-11-07.
//

#include "vulkan_debug.h"

#ifndef NDEBUG
    const uint32_t enable_validation_layers = 1;
#else
    const uint32_t enable_validation_layers = 0;
#endif

const uint32_t validation_layer_count = 1;
const char *validation_layers[] = {"VK_LAYER_KHRONOS_validation"};

VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
VkDebugUtilsMessageTypeFlagsEXT message_type, const VkDebugUtilsMessengerCallbackDataEXT* p_callback_data,
void* p_user_data)
{
    printf("Validation layer: %s\n", p_callback_data->pMessage);
    return VK_FALSE;
}

VkResult create_debug_utils_messenger_EXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* p_create_info, const VkAllocationCallbacks* p_allocator, VkDebugUtilsMessengerEXT* p_debug_messenger) {
    PFN_vkCreateDebugUtilsMessengerEXT func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != NULL) {
        return func(instance, p_create_info, p_allocator, p_debug_messenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void destroy_debug_utils_messenger_EXT(VkInstance instance, VkDebugUtilsMessengerEXT debug_messenger, const VkAllocationCallbacks* p_allocator) {
    PFN_vkDestroyDebugUtilsMessengerEXT func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != NULL) {
        func(instance, debug_messenger, p_allocator);
    }
}

void populate_debug_messenger_create_info(VkDebugUtilsMessengerCreateInfoEXT *p_create_info) {
    p_create_info->sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    p_create_info->messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    p_create_info->messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    p_create_info->pfnUserCallback = debug_callback;
}

uint32_t check_validation_layer_support(void)
{
    uint32_t layer_count;
    vkEnumerateInstanceLayerProperties(&layer_count, NULL);

    
    VkLayerProperties available_layers[layer_count];
    vkEnumerateInstanceLayerProperties(&layer_count, available_layers);

    for(int i = 0; i < validation_layer_count; i++)
    {
        uint32_t check = 0;
        for(int j = 0; j < layer_count; j++)
        {
            if(strcmp(available_layers[j].layerName, validation_layers[i]))
            {
                check = 1;
                break;
            }
        }
        
        if(!check)
        {
            return 0;
        }
    }
    
    return 1;
}

void setup_debug_messenger(VkInstance instance, VkDebugUtilsMessengerEXT *debug_messenger)
{
    if(!enable_validation_layers)
    {
        return;
    }
    
    VkDebugUtilsMessengerCreateInfoEXT create_info = {0};
    populate_debug_messenger_create_info(&create_info);

    if (create_debug_utils_messenger_EXT(instance, &create_info, NULL, debug_messenger) != VK_SUCCESS)
    {
        perror("failed to set up debug messenger!");
    }
}
