

#include "renderer.h"

void setup_context(vulkan_context_t *context, window_t window) {
    dynamic_vector *instance_extension_config = vector_alloc(sizeof(char *));
    get_window_extension_config(instance_extension_config);

    create_instance(&context->instance, instance_extension_config);
    vector_free(instance_extension_config);
#ifndef NDEBUG
    setup_debug_messenger(context->instance, &context->debug_messenger);
#endif
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
        create_image_view(swap_resources->image_views + i, swap_resources->images[i], device_context->logical_device, 1, swap_resources->image_format);
    }
}

void recreate_swap_resources(swap_resources_t *swap_resources, vulkan_context_t *context, device_context_t *device, render_pipeline_t *render_pipeline, window_t window) {
    int width = 0, height = 0;
    //get_framebuffer_size(window, &width, &height);

    vkDeviceWaitIdle(device->logical_device);

    clean_up_swap_resources(swap_resources, device);

    setup_swap_resources(swap_resources, context, device, window);
    for(uint32_t i = 0; i < swap_resources->image_count; i++) {
        create_framebuffer(swap_resources->framebuffers + i, device->logical_device, render_pipeline->render_pass, 1, &swap_resources->image_views[i], swap_resources->extent);
    }
}

void setup_render_pipeline_simple(render_pipeline_t *render_pipeline, device_context_t device_context, swap_resources_t swap_resources) {
    create_render_pass_simple(&render_pipeline->render_pass, device_context.logical_device, swap_resources.image_format);

    VkPipelineLayoutCreateInfo layout_create_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount = 0,
        .pSetLayouts = NULL,
        .pushConstantRangeCount = 0,
        .pPushConstantRanges = NULL
    };

    if(vkCreatePipelineLayout(device_context.logical_device, &layout_create_info, NULL, &render_pipeline->pipeline_layout) != VK_SUCCESS) {
        error(1, "Failed to create pipeline layout\n");
    }

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

    clear_pipeline_details(&details);

    set_input_topology(&details, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
    set_polygon_mode(&details, VK_POLYGON_MODE_FILL);
    set_cull_mode(&details, VK_CULL_MODE_NONE, VK_FRONT_FACE_COUNTER_CLOCKWISE);
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

    VkVertexInputAttributeDescription attribute_description[3];
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

    attribute_description[2] = (VkVertexInputAttributeDescription){
        .binding = 0,
        .location = 2,
        .format = VK_FORMAT_R32G32_SFLOAT,
        .offset = offsetof(vertex_t, texture_coordinates)
    };

    details.vertex_input.vertexBindingDescriptionCount = 1;
    details.vertex_input.pVertexBindingDescriptions = &binding_description;
    details.vertex_input.vertexAttributeDescriptionCount = 3;
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

    create_graphics_pipeline(&render_pipeline->graphics_pipeline, device_context.logical_device, render_pipeline->pipeline_layout, render_pipeline->render_pass, &details);

    vkDestroyShaderModule(device_context.logical_device, vertex_shader, NULL);
    vkDestroyShaderModule(device_context.logical_device, fragment_shader, NULL);
}

void clean_up_context(vulkan_context_t *context) {
#ifndef NDEBUG
    destroy_debug_utils_messenger_EXT(context->instance, context->debug_messenger, NULL);
#endif
    vkDestroySurfaceKHR(context->instance, context->surface, NULL);
    vkDestroyInstance(context->instance, NULL);
}

void clean_up_device_context(device_context_t *device_context) {
    vkDestroyDevice(device_context->logical_device, NULL);
}

void clean_up_swap_resources(swap_resources_t *swap_resources, device_context_t *device_context) {
    for(uint32_t i = 0; i < swap_resources->image_count; i++) {
        vkDestroyFramebuffer(device_context->logical_device, swap_resources->framebuffers[i], NULL);
        vkDestroyImageView(device_context->logical_device, swap_resources->image_views[i], NULL);
    }

    vkDestroySwapchainKHR(device_context->logical_device, swap_resources->swap_chain, NULL);

    free(swap_resources->images);
    free(swap_resources->image_views);
    free(swap_resources->framebuffers);
}

void clean_up_render_pipeline(render_pipeline_t *render_pipeline, device_context_t *device_context) {
    vkDestroyPipeline(device_context->logical_device, render_pipeline->graphics_pipeline, NULL);
    vkDestroyPipelineLayout(device_context->logical_device, render_pipeline->pipeline_layout, NULL);
    vkDestroyRenderPass(device_context->logical_device, render_pipeline->render_pass, NULL);
}

void create_vertex_buffer(buffer_t *vertex_buffer, dynamic_vector *vertex_vector, VkDevice logical_device, VkPhysicalDevice physical_device, VkQueue queue, VkCommandPool command_pool) {
    VkDeviceSize buffer_size = vector_element_size(vertex_vector)*vector_count(vertex_vector);
    buffer_t staging_buffer;

    create_buffer(&staging_buffer.buffer, &staging_buffer.memory, logical_device, physical_device, buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    
    void *data;
    vkMapMemory(logical_device, staging_buffer.memory, 0, buffer_size, 0, &data);
    memcpy(data, vector_get_array(vertex_vector), buffer_size);
    vkUnmapMemory(logical_device, staging_buffer.memory);

    create_buffer(&vertex_buffer->buffer, &vertex_buffer->memory, logical_device, physical_device, buffer_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    copy_buffer(vertex_buffer->buffer, staging_buffer.buffer, logical_device, command_pool, queue, buffer_size);

    destroy_buffer(&staging_buffer, logical_device);
}

void create_index_buffer(buffer_t *index_buffer, dynamic_vector *index_vector, VkDevice logical_device, VkPhysicalDevice physical_device, VkQueue queue, VkCommandPool command_pool) {
    VkDeviceSize buffer_size = vector_element_size(index_vector)*vector_count(index_vector);
    buffer_t staging_buffer;

    create_buffer(&staging_buffer.buffer, &staging_buffer.memory, logical_device, physical_device, buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    
    void *data;
    vkMapMemory(logical_device, staging_buffer.memory, 0, buffer_size, 0, &data);
    memcpy(data, vector_get_array(index_vector), buffer_size);
    vkUnmapMemory(logical_device, staging_buffer.memory);

    create_buffer(&index_buffer->buffer, &index_buffer->memory, logical_device, physical_device, buffer_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    copy_buffer(index_buffer->buffer, staging_buffer.buffer, logical_device, command_pool, queue, buffer_size);

    destroy_buffer(&staging_buffer, logical_device);
}

void destroy_buffer(buffer_t *buffer, VkDevice logical_device) {
    vkDestroyBuffer(logical_device, buffer->buffer, NULL);
    vkFreeMemory(logical_device, buffer->memory, NULL);
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

frame_t *create_frames(VkDevice logical_device, VkCommandPool command_pool, uint32_t frame_count) {
    frame_t *frames = malloc(sizeof(frame_t) * frame_count);
    if(frames == NULL) {
        error(1, "Failed to allocate frames\n");
    }

    for(uint32_t i = 0; i < frame_count; i++) {
        create_frame(frames + i, logical_device, command_pool);
    }

    return frames;
}

void clean_up_frame(frame_t *frame, VkDevice logical_device) {
    vkDestroyFence(logical_device, frame->in_flight_fence, NULL);
    vkDestroySemaphore(logical_device, frame->image_available_semaphore, NULL);
    vkDestroySemaphore(logical_device, frame->render_finished_semaphore, NULL);
}

void clean_up_frames(frame_t *frames, uint32_t frame_count, VkDevice logical_device) {
    for(uint32_t i = 0; i < frame_count; i++) {
        clean_up_frame(frames + i, logical_device);
    }

    free(frames);
}

uint32_t begin_frame(frame_t *frame, VkDevice logical_device, VkSwapchainKHR swap_chain) {
    vkWaitForFences(logical_device, 1, &frame->in_flight_fence, VK_TRUE, UINT64_MAX);

    uint32_t image_index;
    VkResult result = vkAcquireNextImageKHR(logical_device, swap_chain, UINT64_MAX, frame->image_available_semaphore, VK_NULL_HANDLE, &image_index);

    if(result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        error(1, "Failed to acquire swap chain image");
    }

    vkResetFences(logical_device, 1, &frame->in_flight_fence);
    vkResetCommandBuffer(frame->command_buffer, 0);

    return image_index;
}

void end_frame(frame_t *frame, VkSwapchainKHR swap_chain, VkQueue graphics_queue, VkQueue present_queue, uint32_t image_index) {
    VkSemaphore wait_semaphore[] = {frame->image_available_semaphore};
    VkSemaphore signal_semaphore[] = {frame->render_finished_semaphore};
    VkPipelineStageFlags wait_stages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    
    VkSubmitInfo submit_info = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = wait_semaphore,
        .pWaitDstStageMask = wait_stages,
        .commandBufferCount = 1,
        .pCommandBuffers = &frame->command_buffer,
        .signalSemaphoreCount = 1,
        .pSignalSemaphores = signal_semaphore
    };

    if(vkQueueSubmit(graphics_queue, 1, &submit_info, frame->in_flight_fence) != VK_SUCCESS) {
        error(1, "Failed to submit draw command buffer");
    }

    VkSwapchainKHR swap_chains[] = {swap_chain};
    
    VkPresentInfoKHR present_info = {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = signal_semaphore,
        .swapchainCount = 1,
        .pSwapchains = swap_chains,
        .pImageIndices = &image_index,
        .pResults = NULL
    };

    VkResult result = vkQueuePresentKHR(present_queue, &present_info);

    if(result != VK_SUCCESS) {
        error(1, "Failed to present swap chain image");
    }
}

void main_loop(window_t window) {
    while(window_should_close(window)) {
        glfwPollEvents();
    }
}

