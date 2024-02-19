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


const uint32_t frames_in_flight = 2;
extern const uint32_t enable_validation_layers;

#ifdef __APPLE__
    const char device_extension_count = 2;
    const char *device_extensions[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME, "VK_KHR_portability_subset"};
#else
    const char device_extension_count = 1;
    const char *device_extensions[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
#endif


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
        create_frame_buffer(swap_resources.framebuffers + i, device.logical_device, render_pipeline.render_pass, swap_resources.image_views[i], swap_resources.extent);
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
    
    uint32_t N = 37;
    float a = 2*M_PI/N;

    for(uint32_t i = 0; i < N; i++) {
        vertex = (vertex_t){
            .position = {
                .x = cos(a * i),
                .y = sin(a * i),
                .z = 0
            },
            .color = {
                .r = (cos(7*a*i)+1.0f)/2,
                .g = (cos(3*a*i)+1.0f)/2,
                .b = (sin(3*a*i)+1.0f)/2,
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
    while(!window_should_close(window)) {
        glfwPollEvents();
        float d_time = (float)(clock() - time_start)/CLOCKS_PER_SEC;
        image_index = begin_frame(frames + frame_index, device.logical_device, swap_resources.swap_chain);

        begin_command_buffer(frames[frame_index].command_buffer, 0);

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

        vkCmdBeginRenderPass(frames[frame_index].command_buffer, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);
        vkCmdBindPipeline(frames[frame_index].command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, render_pipeline.graphics_pipeline);

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

        vkCmdSetViewport(frames[frame_index].command_buffer, 0, 1, &viewport);
        vkCmdSetScissor(frames[frame_index].command_buffer, 0, 1, &scissor);

        VkBuffer vertex_buffers[] = {vertex_buffer.buffer};
        VkDeviceSize offsets[] = {0};

        vkCmdBindVertexBuffers(frames[frame_index].command_buffer, 0, 1, vertex_buffers, offsets);//Parameters 2, 3 specifies the offsets and how many vertex buffers to bind. Parameters 4, 5 specifies the array of vertex buffers to write and what offset to start reading from
        vkCmdBindIndexBuffer(frames[frame_index].command_buffer, index_buffer.buffer, 0, VK_INDEX_TYPE_UINT16);
        vkCmdDrawIndexed(frames[frame_index].command_buffer, vector_count(index_vector), 1, 0, 0, 0);
        vkCmdEndRenderPass(frames[frame_index].command_buffer);

        end_command_buffer(frames[frame_index].command_buffer);

        end_frame(frames + frame_index, swap_resources.swap_chain, device.queues.graphics_queue, device.queues.graphics_queue, image_index);
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
    clean_up_device_context(&device);
    clean_up_context(&context);
 
    terminate_window(window);
    return 0;
}
