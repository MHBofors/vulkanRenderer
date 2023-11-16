//
//  window.c
//  vulkanRenderer
//
//  Created by Markus Höglin on 2023-10-27.
//

#include "window.h"


const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

void get_window_extension_config(dynamic_vector *window_extension_config) {
    uint32_t available_extension_count = 0;
    vkEnumerateInstanceExtensionProperties(NULL, &available_extension_count, NULL);
    
    VkExtensionProperties available_extensions[available_extension_count];
    vkEnumerateInstanceExtensionProperties(NULL, &available_extension_count, available_extensions);
}

void initialise_window(GLFWwindow **window) {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    
    *window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", NULL, NULL);
}

void terminate_window(GLFWwindow *window) {
    glfwDestroyWindow(window);
    
    glfwTerminate();
}
