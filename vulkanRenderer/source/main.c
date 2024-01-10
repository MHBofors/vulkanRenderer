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
    dynamic_vector *instance_config = vector_alloc(sizeof(const char *));
    dynamic_vector *device_config = vector_alloc(sizeof(const char *));
    dynamic_vector *swap_chain_images = vector_alloc(sizeof(VkImage));
    
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
    VkSwapchainKHR swap_chain;
    
    initialise_window(&window);

    get_window_extension_config(instance_config);

    create_instance(&instance, instance_config);
    setup_debug_messenger(instance, &debug_messenger);
    vector_free(instance_config);
    create_surface(&surface, instance, window);
    select_physical_device(&physical_device, instance, surface);
    create_logical_device(&logical_device, physical_device, surface, &queues, device_config);
    

    VkPresentModeKHR present_mode = choose_swap_present_mode(physical_device, surface);
    VkSurfaceFormatKHR surface_format = choose_swap_surface_format(physical_device, surface);
    VkSurfaceCapabilitiesKHR capabilities; 

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, surface, &capabilities);
    VkExtent2D extent = choose_swap_extent(&capabilities, window);
    create_swap_chain(&swap_chain, logical_device, physical_device, surface, capabilities.minImageCount + 1, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, surface_format, present_mode, extent);
    get_swap_chain_images(swap_chain, logical_device, swap_chain_images);


    struct VkImage_T *a;
    get_swap_chain_images(swap_chain, logical_device, swap_chain_images);

    while(!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }

    vector_free(swap_chain_images);
    vkDestroySwapchainKHR(logical_device, swap_chain, NULL);
    
    vkDestroyDevice(logical_device, NULL);
    
    vkDestroySurfaceKHR(instance, surface, NULL);
    
    if(enable_validation_layers) {
        destroy_debug_utils_messenger_EXT(instance, debug_messenger, NULL);
    }
    vkDestroyInstance(instance, NULL);
    terminate_window(window);
    return 0;
}
