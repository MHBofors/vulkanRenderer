//
//  main.c
//  vulkanRenderer
//
//  Created by Markus Höglin on 2023-10-04.
//



#include "renderer.h"
#include "window.h"


extern const uint32_t enable_validation_layers;

#ifdef __APPLE__
    const char device_extension_count = 2;
    const char *device_extensions[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME, "VK_KHR_portability_subset"};
#else
    const char device_extension_count = 1;
    const char *device_extensions[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
#endif

int main(int argc, const char * argv[]) {
    dynamic_vector *instance_config;
    vector_alloc(&instance_config, sizeof(const char *));
    dynamic_vector *device_config;
    vector_alloc(&device_config, sizeof(const char *));
    dynamic_vector *formats;
    vector_alloc(&formats, sizeof(const char *));
    dynamic_vector *modes;
    vector_alloc(&modes, sizeof(const char *));
    
    for(int i = 0; i < device_extension_count; i++) {
        vector_add(device_config, device_extensions + i);
    }

    GLFWwindow *window;
    VkInstance instance;
    VkDebugUtilsMessengerEXT debug_messenger;
    VkSurfaceKHR surface;
    VkPhysicalDevice physical_device;
    VkDevice logical_device;
    device_queues queues = {0};
    
    initialise_window(&window);

    get_window_extension_config(instance_config);

    create_instance(&instance, instance_config);
    setup_debug_messenger(instance, &debug_messenger);
    vector_free(instance_config);
    create_surface(&surface, instance, window);
    select_physical_device(&physical_device, instance, surface);
    create_logical_device(&logical_device, physical_device, surface, &queues, device_config);
    vector_free(device_config);
    
    query_swap_chain_support(physical_device, surface, formats, modes);

    if(enable_validation_layers) {
        destroy_debug_utils_messenger_EXT(instance, debug_messenger, NULL);
    }

    vkDestroyDevice(logical_device, NULL);
    vkDestroySurfaceKHR(instance, surface, NULL);
    vkDestroyInstance(instance, NULL);
    terminate_window(window);
    return 0;
}
