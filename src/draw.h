#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include "window.h"
#include "vk.h"

void recordCommandBuffer(
    VkCommandBuffer commandBuffer, 
    VkRenderPass renderPass, 
    VkFramebuffer swapChainFramebuffer, 
    VkExtent2D swapChainExtent,
    VkPipelineLayout graphicsPipelineLayout,
    VkPipeline graphicsPipeline,
    const VkDescriptorSet descriptorSet,
    VkBuffer vertexBuffer,
    VkBuffer indexBuffer,
    uint32_t indicesCount
);

void drawFrame(
    VulkanObjects *vko,
    uint32_t currentFrame,
    WindowObjects *wo,
    uint32_t indicesCount
);