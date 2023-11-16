//
//  vulkan_instance.c
//  vulkanRenderer
//
//  Created by Markus Höglin on 2023-11-16.
//


#include "vulkan_instance.h"

uint32_t get_apple_extensions(dynamic_vector *vulkan_extension_config);

void create_instance(VkInstance *p_instance, dynamic_vector *vulkan_extension_config) {
    VkApplicationInfo app_info = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pApplicationName = "Render",
        .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
        .pEngineName = "No Engine",
        .engineVersion = VK_MAKE_VERSION(1, 0, 0),
        .apiVersion = VK_API_VERSION_1_0
    };
    
    uint32_t glfw_extension_count = 0;
    
    const char **glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);
    
    dynamic_vector *extensions;
    vector_alloc(&extensions, sizeof(const char *));
    
    for(int i = 0; i < glfw_extension_count; i++) {
        vector_add(extensions, glfw_extensions + i);
    }

#ifdef __APPLE__
    const uint32_t apple_extension_count = 2;
    const char *apple_extensions[] = {VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME, "VK_KHR_get_physical_device_properties2"};
    for(int i = 0; i < apple_extension_count; i++) {
        vector_add(extensions, apple_extensions + i);
    }
#endif

#ifndef NDEBUG
    const uint32_t debug_extension_count = 1;
    const char *debug_extensions[] =  {VK_EXT_DEBUG_UTILS_EXTENSION_NAME};
    for(int i = 0; i < debug_extension_count; i++) {
        vector_add(extensions, debug_extensions + i);
    }
#endif
    
    VkInstanceCreateInfo create_info = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pApplicationInfo = &app_info,
        .enabledExtensionCount = vector_count(extensions),
        .ppEnabledExtensionNames = (const char **)vector_get_array(extensions),
        .enabledLayerCount = 0,
        .pNext = NULL
    };
    /*
    for(int i = 0; i < vector_count(extensions); i++) {
        printf("%s\n", *(const char **)vector_get_element(extensions, i));
    }
    */
#ifdef __APPLE__
    create_info.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
#endif
    
#ifndef NDEBUG
    VkDebugUtilsMessengerCreateInfoEXT debug_create_info = {0};
    populate_debug_messenger_create_info(&debug_create_info);
    
    create_info.enabledLayerCount = validation_layer_count;
    create_info.ppEnabledLayerNames = validation_layers;
    create_info.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debug_create_info;
#endif
    
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
    
    if(check_extension_support(available_extension_names, available_extension_count, (const char **)vector_get_array(extensions), vector_count(extensions))) {
        printf("Unsupported extensions");
        exit(1);
    }
}