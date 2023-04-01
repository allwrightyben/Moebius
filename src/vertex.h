#pragma once
#include <glm/glm.hpp>
#include <vulkan/vulkan.h>

using namespace glm;

struct Vertex{
    vec2 pos;
    vec3 color;
};
VkVertexInputBindingDescription getBindingDescription();
VkVertexInputAttributeDescription* getAttributeDescriptions(uint32_t *attrDescriptionsSize);
void createVertexBuffer(
    VkDevice device, 
    VkPhysicalDeviceMemoryProperties* memProperties,
    VkCommandPool commandPool,
    VkQueue transferQueue,
    Vertex* vertices, 
    uint32_t verticesSize,
    VkBuffer *vertexBuffer,
    VkDeviceMemory *vertexBufferMemory
);
void createIndexBuffer(
    VkDevice device, 
    VkPhysicalDeviceMemoryProperties* memProperties,
    VkCommandPool transientCommandPool,
    VkQueue transferQueue,
    uint32_t* indices, 
    uint32_t indicesSize,
    VkBuffer *indexBuffer,
    VkDeviceMemory *indexBufferMemory
);