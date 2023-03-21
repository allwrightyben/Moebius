#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <cstdlib>
#include <cstdio>
#include "io.h"
#include "init.h"

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 800;

#ifdef NDEBUG
    const bool ENABLE_VALIDATION_LAYERS = false;
#else
    const bool ENABLE_VALIDATION_LAYERS = true;
#endif

const char* DEVICE_EXTENSIONS[] = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

void recordCommandBuffer(
    VkCommandBuffer commandBuffer, 
    VkRenderPass renderPass, 
    VkFramebuffer* swapChainFramebuffers, 
    uint32_t swapChainFramebuffersIndex,
    VkExtent2D swapChainExtent,
    VkPipeline graphicsPipeline
){
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0;
    beginInfo.pInheritanceInfo = nullptr;

    if(vkBeginCommandBuffer(commandBuffer, &beginInfo)!= VK_SUCCESS){
        printf("Failed to begin Command Buffer Recording!\n");
        exit(EXIT_FAILURE);
    }

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = renderPass;
    renderPassInfo.framebuffer = swapChainFramebuffers[swapChainFramebuffersIndex];
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = swapChainExtent;
    VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)swapChainExtent.width;
    viewport.height = (float)swapChainExtent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = swapChainExtent;
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    vkCmdDraw(commandBuffer, 3, 1, 0, 0);

    vkCmdEndRenderPass(commandBuffer);

    if(vkEndCommandBuffer(commandBuffer) != VK_SUCCESS){
        printf("Failed to end Command Buffer!\n");
        exit(EXIT_FAILURE);
    }
}

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
){
    vkWaitForFences(device, 1, &syncObjects.inFlightFence, VK_TRUE, UINT64_MAX);
    vkResetFences(device, 1, &syncObjects.inFlightFence);

    uint32_t swapchainImageIndex;
    vkAcquireNextImageKHR(device, swapchain, UINT64_MAX, syncObjects.imageAvailableSemaphore, VK_NULL_HANDLE, &swapchainImageIndex);
    
    vkResetCommandBuffer(commandBuffer, 0);
    recordCommandBuffer(commandBuffer, renderPass, swapchainFramebuffers, swapchainImageIndex, swapchainExtent, graphicsPipeline);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &syncObjects.imageAvailableSemaphore;
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.pWaitDstStageMask = waitStages;
    //Each entry in the waitStages array corresponds to the semaphore with the same index in pWaitSemaphores.
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &syncObjects.renderFinishedSemaphore;

    if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, syncObjects.inFlightFence) != VK_SUCCESS) {
        printf("Failed to submit to Graphics Queue!\n");
        exit(EXIT_FAILURE);
    }

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &syncObjects.renderFinishedSemaphore;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &swapchain;
    presentInfo.pImageIndices = &swapchainImageIndex;

    vkQueuePresentKHR(graphicsQueue, &presentInfo);
}

int main(int, char**) {
    printf("Hello World!\n");

    if(!glfwInit()){
        printf("GLFW Init failed!\n");
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Moebius", nullptr, nullptr);
    if(!window){
        printf("GLFW window creation failed!\n");
        exit(EXIT_FAILURE);
    }

    VkInstance instance = createVkInstance(ENABLE_VALIDATION_LAYERS);
    VkSurfaceKHR surface;
    if(glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS){
        printf("Failed to create a Window Surface!");
        exit(EXIT_FAILURE);
    }
    VkPhysicalDevice physicalDevice = pickVkPhysicalDevice(instance);
    VkPhysicalDeviceProperties physicalDeviceProperties;
    vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);
    VkPhysicalDeviceFeatures physicalDeviceFeatures;
    vkGetPhysicalDeviceFeatures(physicalDevice, &physicalDeviceFeatures);
    QueueFamilyIndices queueFamilyIndices = findQueueFamilies(physicalDevice, surface);
    VkDevice device = createLogicalDevice(physicalDevice, &queueFamilyIndices, DEVICE_EXTENSIONS);
    VkQueue graphicsQueue;
    vkGetDeviceQueue(device, queueFamilyIndices.graphics, 0, &graphicsQueue);
    VkQueue presentQueue;
    vkGetDeviceQueue(device, queueFamilyIndices.present, 0, &presentQueue);
    SwapChainSupport swapChainSupport = querySwapChainSupport(physicalDevice, surface);
    VkSurfaceFormatKHR surfaceFormat = selectSurfaceFormat(&swapChainSupport);
    VkPresentModeKHR presentMode = selectPresentMode(&swapChainSupport);
    VkExtent2D swapchainExtent = selectSwapExtent(&swapChainSupport.capabilities, window);
    VkSwapchainKHR swapchain = createSwapChain(device, physicalDevice, surface, window, swapchainExtent, &swapChainSupport.capabilities, surfaceFormat, presentMode);

    VkImage* swapchainImages;
    uint32_t swapchainImageCount;
    vkGetSwapchainImagesKHR(device, swapchain, &swapchainImageCount, nullptr);
    swapchainImages = (VkImage*)malloc(sizeof(VkImage)*swapchainImageCount);
    vkGetSwapchainImagesKHR(device, swapchain, &swapchainImageCount, swapchainImages);
    VkImageView* swapchainImageViews = createImageViews(device, swapchainImages, swapchainImageCount, surfaceFormat);

    VkRenderPass renderPass = createRenderPass(device, surfaceFormat.format);
    VkPipelineLayout graphicsPipelineLayout = createGraphicsPipelineLayout(device);
    VkPipeline graphicsPipeline = createGraphicsPipeline(device, graphicsPipelineLayout, renderPass);

    VkFramebuffer* swapchainFramebuffers = createFramebuffers(device, swapchainImageViews, swapchainImageCount, renderPass, swapchainExtent);

    VkCommandPool commandPool = createCommandPool(device, &queueFamilyIndices);
    VkCommandBuffer commandBuffer = createCommandBuffer(device, commandPool);

    SynchronisationObjects syncObjects = createSyncObjects(device);

    printf("Successfully initialised Vulkan.\n");

    while(!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        drawFrame(device, syncObjects, swapchain, swapchainExtent, commandBuffer, renderPass, swapchainFramebuffers, graphicsPipeline, graphicsQueue);
    }

    vkDeviceWaitIdle(device);

    vkDestroySemaphore(device, syncObjects.imageAvailableSemaphore, nullptr);
    vkDestroySemaphore(device, syncObjects.renderFinishedSemaphore, nullptr);
    vkDestroyFence(device, syncObjects.inFlightFence, nullptr);
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

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
