#include "vk.h"
#include <cstdlib>

void VulkanObjects::cleanUp(){
    vkDestroyBuffer(device, vertexBuffer, nullptr);
    vkFreeMemory(device, vertexBufferMemory, nullptr);
    for(int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++){
        vkDestroySemaphore(device, syncObjects[i].imageAvailableSemaphore, nullptr);
        vkDestroySemaphore(device, syncObjects[i].renderFinishedSemaphore, nullptr);
        vkDestroyFence(device, syncObjects[i].inFlightFence, nullptr);
    }
    vkDestroyCommandPool(device, commandPool, nullptr);
    for(int i = 0; i < swapchainImageCount; i++){
        vkDestroyFramebuffer(device, swapchainFramebuffers[i], nullptr);
        vkDestroyImageView(device, swapchainImageViews[i], nullptr);
    }
    free(swapchainFramebuffers);
    free(swapchainImageViews);
    free(swapchainImages);
    vkDestroyPipeline(device, graphicsPipeline, nullptr);
    vkDestroyPipelineLayout(device, graphicsPipelineLayout, nullptr);
    vkDestroyRenderPass(device, renderPass, nullptr);
    vkDestroySwapchainKHR(device, swapchain, nullptr);
    vkDestroyDevice(device, nullptr);
    vkDestroySurfaceKHR(instance, surface, nullptr);
    vkDestroyInstance(instance, nullptr);
}