//
//  window.c
//  vulkanRenderer
//
//  Created by Markus Höglin on 2023-10-27.
//

#include "window.h"


const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 800;

void create_surface(VkSurfaceKHR *surface, VkInstance instance, window_t window) {
    if (glfwCreateWindowSurface(instance, window, NULL, surface) != VK_SUCCESS) {
        printf("Failed to create window surface!");
        exit(1);
    }
}

void get_framebuffer_size(window_t window, int *width, int *height) {
    glfwGetFramebufferSize(window, width, height);
}


void get_window_extension_config(dynamic_vector *window_extension_config) {
    uint32_t glfw_extension_count = 0;
    const char **glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);
    
    for(int i = 0; i < glfw_extension_count; i++) {
        vector_add(window_extension_config, glfw_extensions + i);
    }
}

VkExtent2D choose_swap_extent(VkSurfaceCapabilitiesKHR *capabilities, window_t window) {
    uint32_t max = ~0;
    if(capabilities->currentExtent.width != max) {
        return capabilities->currentExtent;
    } else {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        
        VkExtent2D actual_extent = {
            .width = (uint32_t)width,
            .height = (uint32_t)height
        };
        
        actual_extent.width = bound(actual_extent.width, capabilities->minImageExtent.width, capabilities->maxImageExtent.width);
        actual_extent.height = bound(actual_extent.height, capabilities->minImageExtent.height, capabilities->maxImageExtent.height);
        return actual_extent;
    }
}

int window_should_close(window_t window) {
    return glfwWindowShouldClose(window);
}

void initialise_window(window_t *window) {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    
    *window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", NULL, NULL);
}

void terminate_window(window_t window) {
    glfwDestroyWindow(window);
    
    glfwTerminate();
}
