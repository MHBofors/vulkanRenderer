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
    printf("hello\n");
    dynamic_vector *instance_config;
    vector_alloc(&instance_config, sizeof(const char *));
    
    GLFWwindow *window;
    VkInstance instance;
    VkPhysicalDevice device;
    VkDebugUtilsMessengerEXT debug_messenger;
    

    
    initialise_window(&window);

    get_window_extension_config(instance_config);
    for(int i = 0; i < vector_count(instance_config); i++) {
        printf("%s\n", *(const char **)vector_get_element(instance_config, i));
    }

    create_instance(&instance, instance_config);
    setup_debug_messenger(instance, &debug_messenger);
    vector_free(instance_config);
    
    
    if (enable_validation_layers) {
        destroy_debug_utils_messenger_EXT(instance, debug_messenger, NULL);
    }
    vkDestroyInstance(instance, NULL);
    terminate_window(window);
    return 0;
}
