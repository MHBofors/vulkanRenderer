

#include "renderer.h"

void setup_context(vulkan_context_t *context, window_t window) {
    dynamic_vector *instance_extension_config = vector_alloc(sizeof(char *));
    get_window_extension_config(instance_extension_config);

    create_instance(&context->instance, instance_extension_config);
    vector_free(instance_extension_config);
    
    setup_debug_messenger(context->instance, &context->debug_messenger);

    create_surface(&context->surface, context->instance, window);
}

void setup_device_context(device_context_t *device_context, vulkan_context_t *context) {
    dynamic_vector *device_config = vector_alloc(sizeof(const char *));

#ifdef __APPLE__
    const char device_extension_count = 2;
    const char *device_extensions[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME, "VK_KHR_portability_subset"};
#else
    const char device_extension_count = 1;
    const char *device_extensions[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
#endif

    for(uint32_t i = 0; i < device_extension_count; i++) {
        vector_add(device_config, device_extensions + i);
    }
    select_physical_device(&device_context->physical_device, context->instance, context->surface);
    create_logical_device(&device_context->logical_device, device_context->physical_device, context->surface, &device_context->queues, device_config);

    vector_free(device_config);
}

void setup_swap_resources(swap_resources_t *swap_resources, vulkan_context_t *vulkan_context, device_context_t *device_context, window_t window) {
    VkSurfaceCapabilitiesKHR capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device_context->physical_device, vulkan_context->surface, &capabilities);
    
    VkPresentModeKHR present_mode = choose_swap_present_mode(device_context->physical_device, vulkan_context->surface);
    VkSurfaceFormatKHR surface_format = choose_swap_surface_format(device_context->physical_device, vulkan_context->surface);
    VkExtent2D extent = choose_swap_extent(&capabilities, window);
    uint32_t min_image_count = capabilities.minImageCount + 1;

    swap_resources->image_format = surface_format.format;
    swap_resources->extent = extent;

    create_swap_chain(&swap_resources->swap_chain, device_context->logical_device, device_context->physical_device, vulkan_context->surface, min_image_count, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, surface_format, present_mode, extent);
    
    vkGetSwapchainImagesKHR(device_context->logical_device, swap_resources->swap_chain, &swap_resources->image_count, NULL);

    swap_resources->images = malloc(sizeof(VkImage) * swap_resources->image_count);
    swap_resources->image_views = malloc(sizeof(VkImageView) * swap_resources->image_count);
    swap_resources->framebuffers = malloc(sizeof(VkFramebuffer) * swap_resources->image_count);

    vkGetSwapchainImagesKHR(device_context->logical_device, swap_resources->swap_chain, &swap_resources->image_count, swap_resources->images);
    
    for(uint32_t i = 0; i < swap_resources->image_count; i++) {
        create_image_view(swap_resources->image_views + i, swap_resources->images[i], device_context->logical_device, swap_resources->image_format);
    }
}

void setup_render_pipeline_simple(render_pipeline_t *render_pipeline, device_context_t device_context, swap_resources_t swap_resources) {
    create_render_pass_simple(&render_pipeline->render_pass, device_context.logical_device, swap_resources.image_format);

    VkShaderModule vertex_shader;
    VkShaderModule fragment_shader;
    load_shader_module(&vertex_shader, device_context.logical_device, "shaders/vert.spv");
    load_shader_module(&fragment_shader, device_context.logical_device, "shaders/frag.spv");

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
        .width = (float) swap_resources.extent.width,
        .height = (float) swap_resources.extent.height,
        .minDepth = 0.0f,
        .maxDepth = 1.0f
    };
    
    //Specifies in which regions pixels will be stored in the framebuffer
    VkRect2D scissor = {
        .offset = {0, 0},
        .extent = swap_resources.extent
    };

    details.viewport.viewportCount = 1;
    details.viewport.pViewports = &viewport;
    details.viewport.scissorCount = 1;
    details.viewport.pScissors = &scissor;

    create_graphics_pipeline(&render_pipeline->graphics_pipeline, &render_pipeline->pipeline_layout, device_context.logical_device, render_pipeline->render_pass, &details);
}

void create_frame(frame_t *frame, VkDevice logical_device, VkCommandPool command_pool) {
    VkSemaphoreCreateInfo semaphore_create_info = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO
    };

    VkFenceCreateInfo fence_create_info = {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .flags = VK_FENCE_CREATE_SIGNALED_BIT
    };

    if(vkCreateSemaphore(logical_device, &semaphore_create_info, NULL, &frame->image_available_semaphore) != VK_SUCCESS |
       vkCreateSemaphore(logical_device, &semaphore_create_info, NULL, &frame->render_finished_semaphore) != VK_SUCCESS |
       vkCreateFence(logical_device, &fence_create_info, NULL, &frame->in_flight_fence) != VK_SUCCESS) {
        error(1, "Failed to create frame sync resources");
    }

    create_primary_command_buffer(&frame->command_buffer, logical_device, command_pool, 1);
}

