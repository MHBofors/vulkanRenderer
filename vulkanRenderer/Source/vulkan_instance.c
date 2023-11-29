//
//  vulkan_instance.c
//  vulkanRenderer
//
//  Created by Markus Höglin on 2023-11-16.
//


#include "vulkan_instance.h"

#ifdef __APPLE__
    const uint32_t enable_apple_support = 1;
#else
    const uint32_t enable_apple_support = 0;
#endif

void apple_instance_support(VkInstanceCreateInfo *create_info, dynamic_vector *instance_extension_config) {
    if(enable_apple_support) {
        const uint32_t apple_extension_count = 2;
        const char *apple_extensions[] = {VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME, "VK_KHR_get_physical_device_properties2"};
        
        for(int i = 0; i < apple_extension_count; i++) {
            vector_add(instance_extension_config, apple_extensions + i);
        }
        create_info->flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
    }
}

void create_instance(VkInstance *p_instance, dynamic_vector *instance_extension_config) {
    VkApplicationInfo app_info = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pApplicationName = "Render",
        .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
        .pEngineName = "No Engine",
        .engineVersion = VK_MAKE_VERSION(1, 0, 0),
        .apiVersion = VK_API_VERSION_1_0
    };

    VkInstanceCreateInfo create_info = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pApplicationInfo = &app_info,
        .flags = 0,
        .pNext = NULL
    };

    apple_instance_support(&create_info, instance_extension_config);

    VkDebugUtilsMessengerCreateInfoEXT debug_create_info = {0};
    create_validation_layers(&create_info, &debug_create_info, instance_extension_config);

    create_info.ppEnabledExtensionNames = (const char **)vector_get_array(instance_extension_config);
    create_info.enabledExtensionCount = vector_count(instance_extension_config);
    
    if(vkCreateInstance(&create_info, NULL, p_instance) != VK_SUCCESS) {
        printf("Failed to create instance");
        exit(1);
    }

    uint32_t available_extension_count = 0;
    vkEnumerateInstanceExtensionProperties(NULL, &available_extension_count, NULL);
    
    VkExtensionProperties available_extensions[available_extension_count];
    vkEnumerateInstanceExtensionProperties(NULL, &available_extension_count, available_extensions);

    const char *available_extension_names[available_extension_count];
    for(int i = 0; i < available_extension_count; i++) {
        available_extension_names[i] = available_extensions[i].extensionName;
    }
    printf("\nRequired instance extensions:\n");
    if(check_extension_support(available_extension_names, available_extension_count, (const char **)vector_get_array(instance_extension_config), vector_count(instance_extension_config))) {
        printf("Unsupported extensions");
        exit(1);
    }
}