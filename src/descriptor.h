#pragma once
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <vulkan/vulkan.h>

using namespace glm;

struct UniformBufferObject
{
    mat4 model;
    mat4 view;
    mat4 proj;
};

VkDescriptorSetLayout createDescriptorSetLayout(VkDevice device);

void createUniformBuffers(
    VkDevice device,
    VkPhysicalDevice physicalDevice,
    uint32_t uniformBufferCount, 
    VkBuffer uniformBuffers[], 
    VkDeviceMemory uniformBuffersMemory[],
    void* uniformBuffersMapped[]
);

void updateUniformBuffer(
    void* mappedUniformBuffer,
    VkExtent2D swapchainExtent
);

VkDescriptorPool createDescriptorPool(VkDevice device, uint32_t uniformBufferCount);

void createDescriptorSets(
    VkDevice device,
    VkDescriptorPool descriptorPool,
    uint32_t descriptorSetCount,
    VkDescriptorSetLayout descriptorSetLayouts[],
    VkBuffer uniformBuffers[],
    VkDescriptorSet *descriptorSets
);