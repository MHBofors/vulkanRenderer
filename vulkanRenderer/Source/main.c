//
//  main.c
//  vulkanRenderer
//
//  Created by Markus Höglin on 2023-10-04.
//



#include "renderer.h"
#include "window.h"


extern const uint32_t enable_validation_layers;

int main(int argc, const char * argv[]) {
    dynamic_vector *instance_config;
    vector_alloc(&instance_config, sizeof(const char *));
    
    GLFWwindow *window;
    VkInstance instance;
    VkDebugUtilsMessengerEXT debug_messenger;
    VkSurfaceKHR surface;
    VkPhysicalDevice physical_device;
    

    
    initialise_window(&window);

    get_window_extension_config(instance_config);
    for(int i = 0; i < vector_count(instance_config); i++) {
        printf("%s\n", *(const char **)vector_get_element(instance_config, i));
    }

    create_instance(&instance, instance_config);
    setup_debug_messenger(instance, &debug_messenger);
    vector_free(instance_config);
    create_surface(&surface, instance, window);
    select_physical_device(&physical_device, instance, surface);
    
    if(enable_validation_layers) {
        destroy_debug_utils_messenger_EXT(instance, debug_messenger, NULL);
    }

    vkDestroySurfaceKHR(instance, surface, NULL);
    vkDestroyInstance(instance, NULL);
    terminate_window(window);
    return 0;
}
