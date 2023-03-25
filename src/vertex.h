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
VkBuffer createVertexBuffer(VkDevice device, Vertex* vertices, uint32_t verticesSize);
VkDeviceMemory allocateVertexBuffer(VkDevice device, VkPhysicalDevice physicalDevice, VkBuffer vertexBuffer);