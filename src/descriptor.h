#pragma once
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
    VkPhysicalDeviceMemoryProperties* memProperties,
    uint32_t uniformBufferCount, 
    VkBuffer *uniformBuffers, 
    VkDeviceMemory *uniformBuffersMemory,
    void* *uniformBuffersMapped
);