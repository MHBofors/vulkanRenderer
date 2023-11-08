//
//  window.h
//  vulkanRenderer
//
//  Created by Markus Höglin on 2023-10-27.
//

#ifndef window_h
#define window_h

#include <stdio.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

void initialise_window(GLFWwindow **window);

void terminate_window(GLFWwindow *window);

#endif /* window_h */
