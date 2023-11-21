//
//  window.h
//  vulkanRenderer
//
//  Created by Markus Höglin on 2023-10-27.
//

#ifndef window_h
#define window_h

#include <stdio.h>
#include "dynamic_vector.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

void create_surface(VkSurfaceKHR *surface, VkInstance instance, GLFWwindow *window);

void get_window_extension_config(dynamic_vector *window_extension_config);

void initialise_window(GLFWwindow **window);

void terminate_window(GLFWwindow *window);

#endif /* window_h */
