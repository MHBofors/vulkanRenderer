//
//  vulkan_device.c
//  vulkanRenderer
//
//  Created by Markus Höglin on 2023-11-06.
//

#include "vulkan_device.h"


const char required_device_extension_count = 1;
const char *required_device_extensions[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

enum queue_family_flag_bits{GRAPHICS_FAMILY_BIT, PRESENT_FAMILY_BIT, QUEUE_FAMILY_FLAG_COUNT};

void select_physical_device(VkInstance instance, VkPhysicalDevice *physical_device) {
    physical_device = VK_NULL_HANDLE;
    
    uint32_t device_count = 0;
    vkEnumeratePhysicalDevices(instance, &device_count, NULL);
    
    if(device_count == 0) {
        printf("Failed to find GPU with Vulkan support!\n");
        exit(1);
    }
    
    
}

uint32_t check_device_suitability(VkPhysicalDevice device, VkSurfaceKHR surface) {
    VkPhysicalDeviceProperties device_properties;
    VkPhysicalDeviceFeatures device_features;
    
    vkGetPhysicalDeviceProperties(device, &device_properties);
    vkGetPhysicalDeviceFeatures(device, &device_features);
    
    queue_family_indices indices = find_queue_families(device, surface);
    
    uint32_t available_extension_count;
    vkEnumerateDeviceExtensionProperties(device, NULL, &available_extension_count, NULL);
    
    VkExtensionProperties available_extensions[available_extension_count];
    vkEnumerateDeviceExtensionProperties(device, NULL, &available_extension_count, available_extensions);
    
    const char *available_extension_names[available_extension_count];
    for(int i = 0; i < available_extension_count; i++) {
        available_extension_names[i] = available_extensions[i].extensionName;
    }
    
    uint32_t extension_support = check_extension_support(available_extension_names, available_extension_count, required_device_extensions, required_device_extension_count);
    
    return extension_support;
}

queue_family_indices find_queue_families(VkPhysicalDevice device, VkSurfaceKHR surface) {
    queue_family_indices indices = {0};

    uint32_t queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, NULL);
    
    VkQueueFamilyProperties queue_families[queue_family_count];
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_families);
    
    for (uint32_t i = 0; i < queue_family_count; i++) {
        if(queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphics_family = i;
            indices.flag_bits |= 1 << GRAPHICS_FAMILY_BIT;
        }
        
        VkBool32 present_support = VK_FALSE;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &present_support);
        if(present_support) {
            indices.graphics_family = i;
            indices.flag_bits |= 1 << PRESENT_FAMILY_BIT;
        }
        
        if(is_complete(indices)) {
            break;
        }
    }
    return indices;
}

uint32_t is_complete(queue_family_indices indices) {
    return indices.flag_bits & ((1 << QUEUE_FAMILY_FLAG_COUNT) - 1);
}
