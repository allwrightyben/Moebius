#include "vk.h"
#include <cstdlib>
#include <cstdio>

void VulkanObjects::cleanUp(){
    vkDestroyBuffer(device, vertexBuffer, nullptr);
    vkFreeMemory(device, vertexBufferMemory, nullptr);
    vkDestroyBuffer(device, indexBuffer, nullptr);
    vkFreeMemory(device, indexBufferMemory, nullptr);
    for(int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++){
        vkDestroySemaphore(device, syncObjects[i].imageAvailableSemaphore, nullptr);
        vkDestroySemaphore(device, syncObjects[i].renderFinishedSemaphore, nullptr);
        vkDestroyFence(device, syncObjects[i].inFlightFence, nullptr);
    }
    vkDestroyCommandPool(device, commandPool, nullptr);
    vkDestroyCommandPool(device, transientCommandPool, nullptr);
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

uint32_t findMemoryType(
    VkPhysicalDeviceMemoryProperties *memProperties, 
    uint32_t typeFilter, 
    VkMemoryPropertyFlags propertyFlags
    ) {
    for (uint32_t i = 0; i < memProperties->memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties->memoryTypes[i].propertyFlags & propertyFlags) == propertyFlags) {
            return i;
        }
    } 

    printf("No suitable GPU Memory Type found!\n");
    exit(EXIT_FAILURE);
}

void createBuffer(
    VkDevice device, 
    VkPhysicalDeviceMemoryProperties memProperties,
    VkBufferUsageFlags usage, 
    VkMemoryPropertyFlags properties,
    VkDeviceSize bufferSize, 
    VkBuffer *buffer, 
    VkDeviceMemory *memory
){
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = bufferSize;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if(vkCreateBuffer(device, &bufferInfo, nullptr, buffer) != VK_SUCCESS){
        printf("Failed to create Buffer of size %lu bytes!\n", bufferSize);
        exit(EXIT_FAILURE);
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device, *buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(
        &memProperties, 
        memRequirements.memoryTypeBits, 
        properties
    );

    if(vkAllocateMemory(device, &allocInfo, nullptr, memory)!=VK_SUCCESS){
        printf("Failed to allocate Buffer Memory!\n");
        exit(EXIT_FAILURE);
    }

    vkBindBufferMemory(device, *buffer, *memory, 0);//If the offset is non-zero, then it is required to be divisible by memRequirements.alignment.
}

void copyBuffer(
    VkDevice device,
    VkCommandPool commandPool,
    VkQueue transferQueue,
    VkBuffer srcBuffer, 
    VkBuffer dstBuffer, 
    VkDeviceSize size
    ){
    
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = commandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    VkBufferCopy copyRegion{};
    copyRegion.srcOffset = 0;
    copyRegion.dstOffset = 0;
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(transferQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(transferQueue);

    vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
}