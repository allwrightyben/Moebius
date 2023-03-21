#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <cstdlib>
#include <cstdio>
#include "io.h"
#include "init.h"
#include "draw.h"

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

struct VulkanObjects{
    VkInstance instance;
    VkSurfaceKHR surface;
    VkPhysicalDevice physicalDevice;
    VkPhysicalDeviceProperties physicalDeviceProperties;
    VkPhysicalDeviceFeatures physicalDeviceFeatures;
    QueueFamilyIndices queueFamilyIndices;
    VkDevice device;
    VkQueue graphicsQueue;
    VkQueue presentQueue;
    VkSurfaceFormatKHR surfaceFormat;
    VkPresentModeKHR presentMode;
    VkExtent2D swapchainExtent;
    VkSwapchainKHR swapchain;
    VkImage* swapchainImages;
    uint32_t swapchainImageCount;
    VkImageView* swapchainImageViews;
    VkRenderPass renderPass; 
    VkPipelineLayout graphicsPipelineLayout;
    VkPipeline graphicsPipeline;
    VkFramebuffer* swapchainFramebuffers;
    VkCommandPool commandPool;
    VkCommandBuffer commandBuffer;
    SynchronisationObjects syncObjects;

    void cleanUp(){
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
    }
};

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

    VulkanObjects vko{};
    vko.instance = createVkInstance(ENABLE_VALIDATION_LAYERS);
    vko.surface;
    if(glfwCreateWindowSurface(vko.instance, window, nullptr, &vko.surface) != VK_SUCCESS){
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
    vko.surfaceFormat = selectSurfaceFormat(&swapChainSupport);
    vko.presentMode = selectPresentMode(&swapChainSupport);
    vko.swapchainExtent = selectSwapExtent(&swapChainSupport.capabilities, window);
    vko.swapchain = createSwapChain(vko.device, vko.physicalDevice, vko.surface, window, vko.swapchainExtent, &swapChainSupport.capabilities, vko.surfaceFormat, vko.presentMode);

    vkGetSwapchainImagesKHR(vko.device, vko.swapchain, &vko.swapchainImageCount, nullptr);
    vko.swapchainImages = (VkImage*)malloc(sizeof(VkImage)*vko.swapchainImageCount);
    vkGetSwapchainImagesKHR(vko.device, vko.swapchain, &vko.swapchainImageCount, vko.swapchainImages);
    vko.swapchainImageViews = createImageViews(vko.device, vko.swapchainImages, vko.swapchainImageCount, vko.surfaceFormat);

    vko.renderPass = createRenderPass(vko.device, vko.surfaceFormat.format);
    vko.graphicsPipelineLayout = createGraphicsPipelineLayout(vko.device);
    vko.graphicsPipeline = createGraphicsPipeline(vko.device, vko.graphicsPipelineLayout, vko.renderPass);

    vko.swapchainFramebuffers = createFramebuffers(vko.device, vko.swapchainImageViews, vko.swapchainImageCount, vko.renderPass, vko.swapchainExtent);

    vko.commandPool = createCommandPool(vko.device, &vko.queueFamilyIndices);
    vko.commandBuffer = createCommandBuffer(vko.device, vko.commandPool);

    vko.syncObjects = createSyncObjects(vko.device);

    printf("Successfully initialised Vulkan.\n");

    while(!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        drawFrame(
            vko.device, 
            vko.syncObjects, 
            vko.swapchain, 
            vko.swapchainExtent, 
            vko.commandBuffer, 
            vko.renderPass, 
            vko.swapchainFramebuffers, 
            vko.graphicsPipeline, 
            vko.graphicsQueue
        );
    }

    vkDeviceWaitIdle(vko.device);

    vko.cleanUp();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
