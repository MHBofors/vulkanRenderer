//
//  main.c
//  vulkanRenderer
//
//  Created by Markus Höglin on 2023-10-04.
//


#include "vulkan_utils.h"

extern const uint32_t enable_validation_layers;

int main(int argc, const char * argv[]) {
    GLFWwindow *window;
    VkInstance instance;
    VkDebugUtilsMessengerEXT debug_messenger;
    
    initialise_window(&window);
    create_instance(&instance);
    setup_debug_messenger(instance, &debug_messenger);
    
    if (enable_validation_layers) {
        destroy_debug_utils_messenger_EXT(instance, debug_messenger, NULL);
    }
    vkDestroyInstance(instance, NULL);
    terminate_window(window);
    return 0;
}
