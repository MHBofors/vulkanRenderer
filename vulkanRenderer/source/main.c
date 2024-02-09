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

typedef struct {
    float x;
    float y;
    float z;
} vector_t;

typedef struct {
    float r;
    float g;
    float b;
    float alpha;
} color_t;

typedef struct {
    vector_t position;
    color_t color;
} vertex_t;

void create_context(vulkan_context_t *context, GLFWwindow **window) {
    initialise_window(window);

    dynamic_vector *instance_config = vector_alloc(sizeof(const char *));
    get_window_extension_config(instance_config);
    create_instance(&context->instance, instance_config);
    vector_free(instance_config);

    setup_debug_messenger(context->instance, &context->debug_messenger);
    create_surface(&context->surface, context->instance, *window);
}

int main(int argc, const char * argv[]) {
    /* Initialization */
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


    uint32_t image_count = capabilities.minImageCount + 1;
    create_swap_chain(&swap_chain, logical_device, physical_device, surface, image_count, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, surface_format, present_mode, extent);
    get_swap_chain_images(swap_chain, logical_device, swap_chain_images);

    VkCommandPool command_pool;
    create_command_pool(&command_pool, logical_device, 0);

    VkRenderPass render_pass;
    create_render_pass_simple(&render_pass, logical_device, surface_format.format);

    VkShaderModule vertex_shader;
    VkShaderModule fragment_shader;
    load_shader_module(&vertex_shader, logical_device, "shaders/vert.spv");
    load_shader_module(&fragment_shader, logical_device, "shaders/frag.spv");

    uint32_t stage_count = 2;
    VkPipelineShaderStageCreateInfo vert_shader_stage_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .stage = VK_SHADER_STAGE_VERTEX_BIT,
        .module = vertex_shader,//Determines the module containing the code
        .pName = "main",//Determines which function will invoke the shader
        .pSpecializationInfo = NULL//Optional member specifying values for shader constants
    };

    VkPipelineShaderStageCreateInfo frag_shader_stage_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
        .module = fragment_shader,
        .pName = "main",
        .pSpecializationInfo = NULL,
    };

    VkPipelineShaderStageCreateInfo shader_modules[2] = {vert_shader_stage_info, frag_shader_stage_info};

    VkPipeline graphics_pipeline;
    VkPipelineLayout pipeline_layout;
    pipeline_details_t details;
    VkPipelineColorBlendAttachmentState blend_attachment;
    details.layout.setLayoutCount = 0;
    details.layout.pSetLayouts = NULL;
    details.layout.pushConstantRangeCount = 0;
    details.layout.pPushConstantRanges = NULL;

    clear_pipeline_details(&details);
    set_input_topology(&details, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
    set_polygon_mode(&details, VK_POLYGON_MODE_FILL);
    set_cull_mode(&details, VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_COUNTER_CLOCKWISE);
    set_multisampler_none(&details);
    set_color_blending_none(&details, &blend_attachment);
    set_blend_attachment_none(&blend_attachment);
    set_depth_test_none(&details);

    details.stage_count = 2;
    details.shader_stages = shader_modules;

    VkVertexInputBindingDescription binding_description = {
        .binding = 0,
        .stride = sizeof(vertex_t),
        .inputRate = VK_VERTEX_INPUT_RATE_VERTEX
    };

    VkVertexInputAttributeDescription attribute_description[2];
    attribute_description[0] = (VkVertexInputAttributeDescription){
        .binding = 0,
        .location = 0,
        .format = VK_FORMAT_R32G32B32_SFLOAT,
        .offset = offsetof(vertex_t, position)
    };
    attribute_description[1] = (VkVertexInputAttributeDescription){
        .binding = 0,
        .location = 1,
        .format = VK_FORMAT_R32G32B32A32_SFLOAT,
        .offset = offsetof(vertex_t, color)
    };

    details.vertex_input.vertexBindingDescriptionCount = 1;
    details.vertex_input.pVertexBindingDescriptions = &binding_description;
    details.vertex_input.vertexAttributeDescriptionCount = 2;
    details.vertex_input.pVertexAttributeDescriptions = attribute_description;

    VkDynamicState dynamic_state[2] = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };

    details.dynamic_state.dynamicStateCount = 2;
    details.dynamic_state.pDynamicStates = dynamic_state;

    VkViewport viewport = {
        .x = 0.0f,
        .y = 0.0f,
        .width = (float) extent.width,
        .height = (float) extent.height,
        .minDepth = 0.0f,
        .maxDepth = 1.0f
    };
    
    //Specifies in which regions pixels will be stored in the framebuffer
    VkRect2D scissor = {
        .offset = {0, 0},
        .extent = extent
    };

    details.viewport.viewportCount = 1;
    details.viewport.pViewports = &viewport;
    details.viewport.scissorCount = 1;
    details.viewport.pScissors = &scissor;

    create_graphics_pipeline(&graphics_pipeline, &pipeline_layout, logical_device, render_pass, &details);

    /* Main-loop */

    while(!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }

    /* Clean-up */

    vkDestroySwapchainKHR(logical_device, swap_chain, NULL);
    vector_free(swap_chain_images);

    vkDestroyCommandPool(logical_device, command_pool, NULL);

    vkDestroyPipeline(logical_device, graphics_pipeline, NULL);

    vkDestroyRenderPass(logical_device, render_pass, NULL);

    vkDestroyDevice(logical_device, NULL);
    
    vkDestroySurfaceKHR(instance, surface, NULL);
    
    if(enable_validation_layers) {
        destroy_debug_utils_messenger_EXT(instance, debug_messenger, NULL);
    }
    vkDestroyInstance(instance, NULL);
    terminate_window(window);
    return 0;
}
