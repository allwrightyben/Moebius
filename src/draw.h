#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include "window.h"
#include "vk.h"

void recordCommandBuffer(
    VkCommandBuffer commandBuffer, 
    VkRenderPass renderPass, 
    VkFramebuffer* swapChainFramebuffers, 
    uint32_t swapChainFramebuffersIndex,
    VkExtent2D swapChainExtent,
    VkPipeline graphicsPipeline,
    VkBuffer vertexBuffer,
    uint32_t verticesCount
);

void drawFrame(
    VulkanObjects *vko,
    uint32_t currentFrame,
    WindowObjects *wo,
    uint32_t verticesCount
);