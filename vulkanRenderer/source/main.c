//
//  main.c
//  vulkanRenderer
//
//  Created by Markus Höglin on 2023-10-04.
//


#include <stdio.h>
#include <math.h>
#include <time.h>
#include "renderer.h"
#include "window.h"
#include <complex.h>


const uint32_t frames_in_flight = 2;
extern const uint32_t enable_validation_layers;

#ifdef __APPLE__
    const char device_extension_count = 2;
    const char *device_extensions[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME, "VK_KHR_portability_subset"};
#else
    const char device_extension_count = 1;
    const char *device_extensions[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
#endif

void create_texture_render_pass(VkRenderPass *render_pass, VkDevice logical_device, VkFormat image_format) {
    VkAttachmentDescription texture_attachment = {
        .format = image_format,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
    };

    VkAttachmentReference texture_attachment_reference = {
        .attachment = 0,
        .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
    };

    VkSubpassDescription subpass = {
        .pipelineBindPoint = VK_PIPELINE_BIND_POINT_COMPUTE,
        .colorAttachmentCount = 1,
        .pColorAttachments = &texture_attachment_reference
    };

    VkSubpassDependency dependency = {
        .srcSubpass = VK_SUBPASS_EXTERNAL,
        .dstSubpass = 0,
        .srcStageMask = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
        .srcAccessMask = 0,
        .dstStageMask = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
        .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT
    };

    VkRenderPassCreateInfo create_info = {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        .attachmentCount = 1,
        .pAttachments = &texture_attachment,
        .subpassCount = 1,
        .pSubpasses = &subpass,
        .dependencyCount = 1,
        .pDependencies = &dependency
    };

    if(vkCreateRenderPass(logical_device, &create_info, NULL, render_pass) != VK_SUCCESS) {
        error(1, "Failed to create render pass");
    }
}

void create_linear_sampler() {
    
}

void create_compute_pipeline(VkPipeline *compute_pipeline, VkPipelineLayout pipeline_layout, VkDevice logical_device, const char *file_name) {
    VkShaderModule compute_shader;
    load_shader_module(&compute_shader, logical_device, file_name);
    VkPipelineShaderStageCreateInfo shader_stage_create_info = create_shader_stage(compute_shader, VK_SHADER_STAGE_COMPUTE_BIT);

    VkComputePipelineCreateInfo create_info = {
        .sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
        .layout = pipeline_layout,
        .stage = shader_stage_create_info,
        .basePipelineHandle = VK_NULL_HANDLE,
        .basePipelineIndex = -1
    };

    if(vkCreateComputePipelines(logical_device, VK_NULL_HANDLE, 1, &create_info, NULL, compute_pipeline) != VK_SUCCESS) {
        error(1, "Failed to create compute pipeline");
    }
}

int main(int argc, const char * argv[]) {
    /* Initialization */
    window_t window;
    vulkan_context_t context;
    device_context_t device;
    swap_resources_t swap_resources;
    render_pipeline_t render_pipeline;
    
    initialise_window(&window);
    setup_context(&context, window);
    setup_device_context(&device, &context);
    setup_swap_resources(&swap_resources, &context, &device, window);
    setup_render_pipeline_simple(&render_pipeline, device, swap_resources);

    for(uint32_t i = 0; i < swap_resources.image_count; i++) {
        create_frame_buffer(swap_resources.framebuffers + i, device.logical_device, render_pipeline.render_pass, 1, swap_resources.image_views + i, swap_resources.extent);
    }

    image_t fractal_images[frames_in_flight];
    VkImageView fractal_image_views[frames_in_flight];
    VkFramebuffer fractal_frame_buffer[frames_in_flight];

    VkRenderPass compute_render_pass;
    VkDescriptorSetLayout compute_pipeline_layout;

    VkMemoryBarrier compute_pipeline_barriers;
    
    for(uint32_t i = 0; i < frames_in_flight; i++) {
        create_image(&fractal_images[i].image, &fractal_images[i].memory, device.logical_device, device.physical_device, swap_resources.extent.width,  swap_resources.extent.height, 1, VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_R32_SFLOAT, VK_IMAGE_TILING_LINEAR, VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        create_image_view(fractal_image_views + i, fractal_images[i].image, device.logical_device, 1, VK_FORMAT_R32_SFLOAT);
        
        VkImageView image_views[2] = {swap_resources.image_views[i], fractal_image_views[i]};
    }

    queue_family_indices indices = find_queue_families(device.physical_device);
    VkCommandPool command_pool;
    create_command_pool(&command_pool, device.logical_device, indices.graphics_family);
    frame_t *frames = create_frames(device.logical_device, command_pool, frames_in_flight);

    buffer_t vertex_buffer;
    buffer_t index_buffer;
    dynamic_vector *vertex_vector = vector_alloc(sizeof(vertex_t));
    dynamic_vector *index_vector = vector_alloc(sizeof(uint16_t));

    vertex_t vertex;
    
    uint32_t N = 256;
    float a = 2*M_PI/N;

    for(uint32_t i = 0; i < N; i++) {
        vertex = (vertex_t){
            .position = {
                .x = cos(a * i),
                .y = sin(a * i),
                .z = 0
            },
            .color = {
                .r = (cos(13*a*i+5)+1.0f)/4 + 0.25,
                .g = (cos(17*a*i+7)+1.0f)/4 + 0.25,
                .b = (sin(19*a*i+11)+1.0f)/4 + 0.35,
                .alpha = 1.0f
            }
        };
        vector_add(vertex_vector, &vertex);

        uint16_t triangle[3] = {i, (i + 1) % N, N+1};
        vector_add(index_vector, triangle + 0);
        vector_add(index_vector, triangle + 1);
        vector_add(index_vector, triangle + 2);
    }

    vertex = (vertex_t){
        .position = {
            .x = 0,
            .y = 0,
            .z = 0
        },
        .color = {
            .r = 1.0f,
            .g = 0,
            .b = 0,
            .alpha = 1.0f
        }
    };
    vector_add(vertex_vector, &vertex);

    create_vertex_buffer(&vertex_buffer, vertex_vector, device.logical_device, device.physical_device, device.queues.graphics_queue, command_pool);
    create_index_buffer(&index_buffer, index_vector, device.logical_device, device.physical_device, device.queues.graphics_queue, command_pool);
    /* Main-loop */

    uint32_t image_index;
    uint32_t frame_index = 0;

    clock_t time_start = clock();
    
    frame_t *frame;
    VkCommandBuffer command_buffer;
    while(!window_should_close(window)) {
        glfwPollEvents();
        float d_time = (float)(clock() - time_start)/CLOCKS_PER_SEC;

        frame = frames + frame_index;
        command_buffer = frame->command_buffer;

        image_index = begin_frame(frame, device.logical_device, swap_resources.swap_chain);

        begin_command_buffer(command_buffer, 0);

        VkClearValue clear_color = {{{0.0f, 0.0f, 0.0f, 1.0f}}};

        VkRenderPassBeginInfo render_pass_info = {
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
            .renderPass = render_pipeline.render_pass,
            .framebuffer = swap_resources.framebuffers[image_index],
            .renderArea = {
                .offset = {0, 0},
                .extent = swap_resources.extent
            },
            .clearValueCount = 1,
            .pClearValues = &clear_color
        };

        vkCmdBeginRenderPass(command_buffer, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);
        vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, render_pipeline.graphics_pipeline);

        VkViewport viewport = {
            .x = 0.0f,
            .y = 0.0f,
            .width = swap_resources.extent.width,
            .height = swap_resources.extent.height,
            .minDepth = 0.0f,
            .maxDepth = 1.0f
        };

        VkRect2D scissor = {
            .offset = {0, 0},
            .extent = swap_resources.extent
        };

        vkCmdSetViewport(command_buffer, 0, 1, &viewport);
        vkCmdSetScissor(command_buffer, 0, 1, &scissor);

        VkBuffer vertex_buffers[] = {vertex_buffer.buffer};
        VkDeviceSize offsets[] = {0};

        vkCmdBindVertexBuffers(command_buffer, 0, 1, vertex_buffers, offsets);//Parameters 2, 3 specifies the offsets and how many vertex buffers to bind. Parameters 4, 5 specifies the array of vertex buffers to write and what offset to start reading from
        vkCmdBindIndexBuffer(command_buffer, index_buffer.buffer, 0, VK_INDEX_TYPE_UINT16);
        vkCmdDrawIndexed(command_buffer, vector_count(index_vector), 1, 0, 0, 0);
        vkCmdEndRenderPass(command_buffer);

        end_command_buffer(command_buffer);

        end_frame(frame, swap_resources.swap_chain, device.queues.graphics_queue, device.queues.graphics_queue, image_index);
        frame_index = (frame_index + 1) % frames_in_flight;
    }

    vkDeviceWaitIdle(device.logical_device);

    /* Clean-up */

    clean_up_swap_resources(&swap_resources, &device);
    destroy_buffer(&vertex_buffer, device.logical_device);
    destroy_buffer(&index_buffer, device.logical_device);
    clean_up_frames(frames, frames_in_flight, device.logical_device);
    vkDestroyCommandPool(device.logical_device, command_pool, NULL);
    clean_up_render_pipeline(&render_pipeline, &device);
    for(uint32_t i = 0; i < frames_in_flight; i++) {
        vkFreeMemory(device.logical_device, fractal_images[i].memory, NULL);
        vkDestroyImage(device.logical_device, fractal_images[i].image, NULL);
        vkDestroyImageView(device.logical_device, fractal_image_views[i], NULL);
    }
    clean_up_device_context(&device);
    clean_up_context(&context);
 
    terminate_window(window);
    return 0;
}
