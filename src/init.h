#pragma once
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <vector>

struct QueueFamilyIndices{
    uint32_t graphics = UINT32_MAX;
    uint32_t present = UINT32_MAX;
};

struct SynchronisationObjects{
    VkSemaphore imageAvailableSemaphore;
    VkSemaphore renderFinishedSemaphore;
    VkFence inFlightFence;
};

struct SwapChainSupport {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

VkInstance createVkInstance(bool validationLayersEnabled);
VkPhysicalDevice pickVkPhysicalDevice(VkInstance instance);
QueueFamilyIndices findQueueFamilies(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
VkDevice createLogicalDevice(VkPhysicalDevice physicalDevice, QueueFamilyIndices *indices, const char* deviceExtensions[]);
SwapChainSupport querySwapChainSupport(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
VkSurfaceFormatKHR selectSurfaceFormat(SwapChainSupport *support);
VkPresentModeKHR selectPresentMode(SwapChainSupport *support);
VkExtent2D selectSwapExtent(const VkSurfaceCapabilitiesKHR *capabilities, GLFWwindow *window);
VkSwapchainKHR createSwapChain(VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, GLFWwindow *window, VkExtent2D extent, VkSurfaceCapabilitiesKHR *capabilities, VkSurfaceFormatKHR surfaceFormat, VkPresentModeKHR presentMode);
VkShaderModule createShaderModule(VkDevice device, char* code, size_t codeSize);
VkRenderPass createRenderPass(VkDevice device, VkFormat swapChainImageFormat);
VkPipeline createGraphicsPipeline(VkDevice device, VkPipelineLayout pipelineLayout, VkRenderPass renderPass);
VkPipelineLayout createGraphicsPipelineLayout(VkDevice device);
VkImageView* createImageViews(
    VkDevice device, 
    VkImage swapchainImages[], 
    uint32_t swapchainImageCount, 
    VkSurfaceFormatKHR surfaceFormat
    );
VkFramebuffer* createFramebuffers(VkDevice device, VkImageView* swapchainImageViews, int swapchainImageCount, VkRenderPass renderPass, VkExtent2D swapchainExtent);
VkCommandPool createCommandPool(VkDevice device, QueueFamilyIndices *queueFamilyIndices);
VkCommandBuffer createCommandBuffer(VkDevice device, VkCommandPool commandPool);
SynchronisationObjects createSyncObjects(VkDevice device);