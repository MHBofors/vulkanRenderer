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


const uint32_t frames_in_flight = 3;
extern const uint32_t enable_validation_layers;



#ifdef __APPLE__
    const char device_extension_count = 3;
    const char *device_extensions[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME, "VK_KHR_portability_subset"};
#else
    const char device_extension_count = 2;
    const char *device_extensions[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
#endif

void create_linear_sampler(VkSampler *sampler, VkDevice logical_device) {
    VkSamplerCreateInfo create_info = {
        .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
        .magFilter = VK_FILTER_LINEAR,
        .minFilter = VK_FILTER_LINEAR
    };
	
	vkCreateSampler(logical_device, &create_info, NULL, sampler);
}

void create_compute_pipeline_layout(VkPipelineLayout *pipeline_layout, VkDevice logical_device, VkDescriptorSetLayout layout) {
    VkPushConstantRange push_constant_range = {
        .stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
        .offset = 0,
        .size = 12
    };

    VkPipelineLayoutCreateInfo create_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount = 1,
        .pSetLayouts = &layout,
        .pushConstantRangeCount = 1,
        .pPushConstantRanges = &push_constant_range
    };

    if(vkCreatePipelineLayout(logical_device, &create_info, NULL, pipeline_layout) != VK_SUCCESS) {
        error(1, "Failed to create compute pipeline layout");
    }
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

    vkDestroyShaderModule(logical_device, compute_shader, NULL);
}

void setup_graphics_pipeline_layout(VkPipelineLayout *pipeline_layout, VkDevice logical_device, VkDescriptorSetLayout layout) {
    VkPipelineLayoutCreateInfo create_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount = 1,
        .pSetLayouts = &layout,
        .pushConstantRangeCount = 0,
        .pPushConstantRanges = NULL
    };

    if(vkCreatePipelineLayout(logical_device, &create_info, NULL, pipeline_layout) != VK_SUCCESS) {
        error(1, "Failed to create graphics pipeline layout");
    }
}

void setup_graphics_pipeline(VkPipeline *graphics_pipeline, VkPipelineLayout pipeline_layout, VkRenderPass render_pass, device_context_t device_context, swap_resources_t swap_resources) {
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

    create_graphics_pipeline(graphics_pipeline, device_context.logical_device, pipeline_layout, render_pass, &details);

    vkDestroyShaderModule(device_context.logical_device, vertex_shader, NULL);
    vkDestroyShaderModule(device_context.logical_device, fragment_shader, NULL);
}

int main(int argc, const char * argv[]) {
    /* Initialization */
    float phi = (1.0 + sqrt(5.0))*0.5;
    float phi_1 = (sqrt(5.0) - 1)*0.5;

    window_t window;
    vulkan_context_t context;
    device_context_t device;
    swap_resources_t swap_resources;
    render_pipeline_t render_pipeline;
    
    initialise_window(&window);
    setup_context(&context, window);
    setup_device_context(&device, &context);
    setup_swap_resources(&swap_resources, &context, &device, window);
    create_render_pass_simple(&render_pipeline.render_pass, device.logical_device, swap_resources.image_format);
    

    for(uint32_t i = 0; i < swap_resources.image_count; i++) {
        create_framebuffer(swap_resources.framebuffers + i, device.logical_device, render_pipeline.render_pass, 1, swap_resources.image_views + i, swap_resources.extent);
    }

    host_buffer_t uniform_buffers[frames_in_flight];

    image_t fractal_images[frames_in_flight];
    VkImageView fractal_image_views[frames_in_flight];
    VkSampler sampler;

    create_linear_sampler(&sampler, device.logical_device);

    VkImageMemoryBarrier compute_pipeline_barriers[frames_in_flight][2];

    VkPipelineLayout compute_pipeline_layout;
    VkPipeline compute_pipeline;

    VkDescriptorPool descriptor_pool;

    VkDescriptorPoolSize image_pool_size = {
        .type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
        .descriptorCount = frames_in_flight
    };

    VkDescriptorPoolSize texture_pool_size = {
        .type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
        .descriptorCount = frames_in_flight
    };

    VkDescriptorPoolSize sampler_pool_size = {
        .type = VK_DESCRIPTOR_TYPE_SAMPLER,
        .descriptorCount = frames_in_flight
    };

    VkDescriptorPoolSize buffer_pool_size = {
        .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .descriptorCount = frames_in_flight
    };

    VkDescriptorPoolSize pool_sizes[4] = {image_pool_size, texture_pool_size, sampler_pool_size, buffer_pool_size};

    create_descriptor_pool(&descriptor_pool, device.logical_device, pool_sizes, 4, 256);

    VkDescriptorSetLayoutBinding image_binding = {
        .stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
        .binding = 0,
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE
    };

    VkDescriptorSetLayoutBinding buffer_binding = {
        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
        .binding = 0,
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
    };

    VkDescriptorSetLayoutBinding texture_binding = {
        .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
        .binding = 1,
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE
    };

    VkDescriptorSetLayoutBinding sampler_binding = {
        .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
        .binding = 2,
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER
    };

    VkDescriptorSetLayoutBinding bindings[] = {buffer_binding, texture_binding, sampler_binding};

    VkDescriptorSetLayout image_layout;
    VkDescriptorSetLayout descriptor_layout;
    create_descriptor_set_layout(&image_layout, device.logical_device, &image_binding, 1);
    create_descriptor_set_layout(&descriptor_layout, device.logical_device, &bindings, 3);

    setup_graphics_pipeline_layout(&render_pipeline.pipeline_layout, device.logical_device, descriptor_layout);
    setup_graphics_pipeline(&render_pipeline.graphics_pipeline, render_pipeline.pipeline_layout, render_pipeline.render_pass, device, swap_resources);

    VkDescriptorSet compute_descriptor_sets[frames_in_flight];
    VkDescriptorSetLayout layouts[frames_in_flight];
    for(uint32_t i = 0; i < frames_in_flight; i++) {
        layouts[i] = image_layout;
    }
    
    
    create_compute_pipeline_layout(&compute_pipeline_layout, device.logical_device, image_layout);
    create_compute_pipeline(&compute_pipeline, compute_pipeline_layout, device.logical_device, "shaders/compute.spv");

    for(uint32_t i = 0; i < frames_in_flight; i++) {
        create_image(&fractal_images[i].image, &fractal_images[i].memory, device.logical_device, device.physical_device, 1024, 1024, 1, VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_R32G32B32A32_SFLOAT, VK_IMAGE_TILING_LINEAR, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_DEVICE_COHERENT_BIT_AMD);
        create_image_view(fractal_image_views + i, fractal_images[i].image, device.logical_device, 1, VK_FORMAT_R32G32B32A32_SFLOAT);
        
        create_buffer(&uniform_buffers[i], &uniform_buffers[i].memory, device.logical_device, device.physical_device, (VkDeviceSize)3*sizeof(float[4][4]), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        compute_pipeline_barriers[i][0] = (VkImageMemoryBarrier){
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
            .image = fractal_images[i].image,
            .subresourceRange = (VkImageSubresourceRange){
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .levelCount = 1,
                .baseMipLevel = 0,
                .baseArrayLayer = 0,
                .layerCount = 1
            },
            .srcAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT,
            .dstAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT,
            .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .newLayout = VK_IMAGE_LAYOUT_GENERAL,
            .pNext = NULL
        };

        compute_pipeline_barriers[i][1] = (VkImageMemoryBarrier){
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
            .image = fractal_images[i].image,
            .subresourceRange = (VkImageSubresourceRange){
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .levelCount = 1,
                .baseMipLevel = 0,
                .baseArrayLayer = 0,
                .layerCount = 1
            },
            .srcAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT,
            .dstAccessMask = VK_ACCESS_2_SHADER_READ_BIT,
            .oldLayout = VK_IMAGE_LAYOUT_GENERAL,
            .newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            .pNext = NULL
        };

        VkImageView image_views[1] = {fractal_image_views[i]};
    }

    allocate_descriptor_set(compute_descriptor_sets, device.logical_device, descriptor_pool, layouts, frames_in_flight);
    
    for(uint32_t i = 0; i < frames_in_flight; i++) {
        VkDescriptorImageInfo image_info = {
            .imageLayout = VK_IMAGE_LAYOUT_GENERAL,
            .imageView = fractal_image_views[i]
        };

        VkWriteDescriptorSet image_write = {
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
            .dstBinding = 0,
            .dstSet = compute_descriptor_sets[i],
            .descriptorCount = 1,
            .pImageInfo = &image_info
        };

        update_descriptor_set(device.logical_device, compute_descriptor_sets[i], &image_write, 1);
    }

    for(uint32_t i = 0; i < frames_in_flight; i++) {
        layouts[i] = descriptor_layout;
    }

    VkDescriptorSet graphics_descriptor_sets[frames_in_flight];
    allocate_descriptor_set(graphics_descriptor_sets, device.logical_device, descriptor_pool, layouts, frames_in_flight);

    for(uint32_t i = 0; i < frames_in_flight; i++) {
        VkDescriptorBufferInfo buffer_info = {
            .buffer = uniform_buffers[i].buffer,
            .offset = 0,
            .range = 3*sizeof(float[4][4])
        };

        VkWriteDescriptorSet buffer_write = {
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .dstBinding = 0,
            .dstSet = graphics_descriptor_sets[i],
            .descriptorCount = 1,
            .pBufferInfo = &buffer_info
        };

        VkDescriptorImageInfo image_info = {
            .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            .imageView = fractal_image_views[i],
        };

        VkWriteDescriptorSet image_write = {
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
            .dstBinding = 1,
            .dstSet = graphics_descriptor_sets[i],
            .descriptorCount = 1,
            .pImageInfo = &image_info
        };

        VkDescriptorImageInfo sampler_info = {
            .sampler = sampler
        };

        VkWriteDescriptorSet sampler_write = {
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER,
            .dstBinding = 2,
            .dstSet = graphics_descriptor_sets[i],
            .descriptorCount = 1,
            .pImageInfo = &sampler_info
        };

        VkWriteDescriptorSet writes[] = {buffer_write, image_write, sampler_write};
        update_descriptor_set(device.logical_device, graphics_descriptor_sets[i], writes, 3);
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
    /*
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
            },
            .texture_coordinates = {
                .u = cos(a * i),
                .v = sin(a * i)
            }
        };
        vector_add(vertex_vector, &vertex);

        uint16_t triangle[3] = {i, (i + 1) % N, N+1};
        vector_add(index_vector, triangle + 0);
        vector_add(index_vector, triangle + 1);
        vector_add(index_vector, triangle + 2);
    }
    */
    vertex = (vertex_t){
        .position = {
            .x =-1.0,
            .y =-1.0,
            .z = 0.0
        },
        .color = {
            .r = 1.0f,
            .g = 0,
            .b = 0,
            .alpha = 1.0f
        },
        .texture_coordinates = {
            .u = 0,
            .v = 0
        }
    };
    vector_add(vertex_vector, &vertex);
    vertex = (vertex_t){
        .position = {
            .x = 1.0,
            .y =-1.0,
            .z = 0.0
        },
        .color = {
            .r = 1.0f,
            .g = 0,
            .b = 0,
            .alpha = 1.0f
        },
        .texture_coordinates = {
            .u = 1,
            .v = 0
        }
    };
    vector_add(vertex_vector, &vertex);
    vertex = (vertex_t){
        .position = {
            .x = 1.0,
            .y = 1.0,
            .z = 0.0
        },
        .color = {
            .r = 1.0f,
            .g = 0,
            .b = 0,
            .alpha = 1.0f
        },
        .texture_coordinates = {
            .u = 1,
            .v = 1
        }
    };
    vector_add(vertex_vector, &vertex);
    vertex = (vertex_t){
        .position = {
            .x =-1.0,
            .y = 1.0,
            .z = 0.0
        },
        .color = {
            .r = 1.0f,
            .g = 0,
            .b = 0,
            .alpha = 1.0f
        },
        .texture_coordinates = {
            .u = 0,
            .v = 1
        }
    };
    vector_add(vertex_vector, &vertex);

    uint16_t triangle[6] = {0, 1, 2, 3};
    vector_add(index_vector, triangle + 0);
    vector_add(index_vector, triangle + 1);
    vector_add(index_vector, triangle + 3);
    vector_add(index_vector, triangle + 2);
    vector_add(index_vector, triangle + 3);
    vector_add(index_vector, triangle + 1);

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
        float t = (float)(clock() - time_start)/CLOCKS_PER_SEC;

        frame = frames + frame_index;
        command_buffer = frame->command_buffer;

        image_index = begin_frame(frame, device.logical_device, swap_resources.swap_chain);

        begin_command_buffer(command_buffer, VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT);

        VkDependencyInfo dependency_info = {
            .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
            .imageMemoryBarrierCount = 0,
            .pImageMemoryBarriers = &compute_pipeline_barriers[frame_index],
            .memoryBarrierCount = 0,
            .pMemoryBarriers = NULL,
            .bufferMemoryBarrierCount = 0,
            .pBufferMemoryBarriers = NULL,
            .pNext = NULL
        };
        
        vkCmdPipelineBarrier(command_buffer, VK_PIPELINE_STAGE_HOST_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, 0, 0, NULL, 0, NULL, 1, &compute_pipeline_barriers[frame_index][0]);
        vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_COMPUTE, compute_pipeline);
        vkCmdBindDescriptorSets(command_buffer, VK_PIPELINE_BIND_POINT_COMPUTE, compute_pipeline_layout, 0, 1, &compute_descriptor_sets[frame_index], 0, NULL);
        

        t *= 0.0625;
        //t *= 0.125;
        float z[2] = {(cos(t) - cos(2.00*t)*0.5)*0.5, (sin(t) - sin(2.00*t)*0.5)*0.5};
        z[0] *= 1.0 + 0.01;
        z[1] *= 1.0 + 0.01;
        

        
        //float z[2] = {(cos(t) - cos(2.0*t)*0.5)*0.5+0.00625*cos(0.125*t), (sin(t) - sin(2.0*t)*0.5+0.00625*sin(0.125*t))*0.5};
        vkCmdPushConstants(command_buffer, compute_pipeline_layout, VK_SHADER_STAGE_COMPUTE_BIT, 0, 8, z);
        vkCmdDispatch(command_buffer, 64, 64, 1);

        vkCmdPipelineBarrier(command_buffer, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, NULL, 0, NULL, 1, &compute_pipeline_barriers[frame_index][1]);

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
            .pClearValues = &clear_color,
            .pNext = NULL
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
        
        vkCmdBindDescriptorSets(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, render_pipeline.pipeline_layout, 0, 1, &graphics_descriptor_sets[frame_index], 0, NULL);
        
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
        vkFreeMemory(device.logical_device, uniform_buffers[i].memory, NULL);
        vkDestroyBuffer(device.logical_device, uniform_buffers[i].buffer, NULL);
    }

    vkDestroyPipelineLayout(device.logical_device, compute_pipeline_layout, NULL);
    vkDestroyPipeline(device.logical_device, compute_pipeline, NULL);
    vkDestroySampler(device.logical_device, sampler, NULL);
    vkDestroyDescriptorSetLayout(device.logical_device, image_layout, NULL);
    vkDestroyDescriptorSetLayout(device.logical_device, descriptor_layout, NULL);
    vkDestroyDescriptorPool(device.logical_device, descriptor_pool, NULL);
    
    clean_up_device_context(&device);
    clean_up_context(&context);
    
    terminate_window(window);
    return 0;
}
