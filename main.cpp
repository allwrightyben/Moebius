#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <cstdlib>
#include <cstdio>
#include "window.h"
#include "vk.h"
#include "io.h"
#include "initvk.h"
#include "draw.h"

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

#ifdef NDEBUG
    const bool ENABLE_VALIDATION_LAYERS = false;
#else
    const bool ENABLE_VALIDATION_LAYERS = true;
#endif

const char* DEVICE_EXTENSIONS[] = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

int main(int, char**) {
    printf("Hello World!\n");

    WindowObjects wo{};
    initGLFWWindow(&wo, WIDTH, HEIGHT);

    VulkanObjects vko{};
    vko.instance = createVkInstance(ENABLE_VALIDATION_LAYERS);
    if(glfwCreateWindowSurface(vko.instance, wo.window, nullptr, &vko.surface) != VK_SUCCESS){
        printf("Failed to create a Window Surface!");
        exit(EXIT_FAILURE);
    }
    vko.physicalDevice = pickVkPhysicalDevice(vko.instance);
    vkGetPhysicalDeviceProperties(vko.physicalDevice, &vko.physicalDeviceProperties);
    vkGetPhysicalDeviceFeatures(vko.physicalDevice, &vko.physicalDeviceFeatures);
    vko.queueFamilyIndices = findQueueFamilies(vko.physicalDevice, vko.surface);
    vko.device = createLogicalDevice(vko.physicalDevice, &vko.queueFamilyIndices, DEVICE_EXTENSIONS);
    vkGetDeviceQueue(vko.device, vko.queueFamilyIndices.graphics, 0, &vko.graphicsQueue);
    vkGetDeviceQueue(vko.device, vko.queueFamilyIndices.present, 0, &vko.presentQueue);
    SwapChainSupport swapChainSupport = querySwapChainSupport(vko.physicalDevice, vko.surface);
    vko.capabilities = swapChainSupport.capabilities;
    vko.surfaceFormat = selectSurfaceFormat(&swapChainSupport);
    vko.presentMode = selectPresentMode(&swapChainSupport);
    vko.swapchainExtent = selectSwapchainExtent(&swapChainSupport.capabilities, wo.window);
    vko.swapchain = createSwapChain(vko.device, vko.physicalDevice, vko.surface, wo.window, vko.swapchainExtent, &swapChainSupport.capabilities, vko.surfaceFormat, vko.presentMode);

    vkGetSwapchainImagesKHR(vko.device, vko.swapchain, &vko.swapchainImageCount, nullptr);
    vko.swapchainImages = (VkImage*)malloc(sizeof(VkImage)*vko.swapchainImageCount);
    vkGetSwapchainImagesKHR(vko.device, vko.swapchain, &vko.swapchainImageCount, vko.swapchainImages);
    vko.swapchainImageViews = createImageViews(vko.device, vko.swapchainImages, vko.swapchainImageCount, vko.surfaceFormat);

    vko.renderPass = createRenderPass(vko.device, vko.surfaceFormat.format);
    vko.graphicsPipelineLayout = createGraphicsPipelineLayout(vko.device);
    vko.graphicsPipeline = createGraphicsPipeline(vko.device, vko.graphicsPipelineLayout, vko.renderPass);

    vko.swapchainFramebuffers = createFramebuffers(vko.device, vko.swapchainImageViews, vko.swapchainImageCount, vko.renderPass, vko.swapchainExtent);

    vko.commandPool = createCommandPool(vko.device, &vko.queueFamilyIndices);
    createCommandBuffers(vko.device, vko.commandPool, MAX_FRAMES_IN_FLIGHT, vko.commandBuffers);

    for(int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++){
        vko.syncObjects[i] = createSyncObjects(vko.device);
    }

    printf("Successfully initialised Vulkan.\n");

    uint32_t currentFrame = 0;

    while(!glfwWindowShouldClose(wo.window)) {
        glfwPollEvents();
        drawFrame(
            &vko,
            currentFrame,
            &wo
        );

        currentFrame = 1 - currentFrame;
    }

    vkDeviceWaitIdle(vko.device);

    vko.cleanUp();

    glfwDestroyWindow(wo.window);
    glfwTerminate();

    return 0;
}
