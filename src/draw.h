#pragma once
#include <vulkan/vulkan.h>
#include "init.h"

void recordCommandBuffer(
    VkCommandBuffer commandBuffer, 
    VkRenderPass renderPass, 
    VkFramebuffer* swapChainFramebuffers, 
    uint32_t swapChainFramebuffersIndex,
    VkExtent2D swapChainExtent,
    VkPipeline graphicsPipeline
);

void drawFrame(
    VkDevice device, 
    SynchronisationObjects syncObjects, 
    VkSwapchainKHR swapchain, 
    VkExtent2D swapchainExtent,
    VkCommandBuffer commandBuffer, 
    VkRenderPass renderPass,
    VkFramebuffer *swapchainFramebuffers,
    VkPipeline graphicsPipeline,
    VkQueue graphicsQueue
);