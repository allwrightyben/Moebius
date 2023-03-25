#include "vertex.h"
#include <cstdio>
#include "vk.h"

VkVertexInputBindingDescription getBindingDescription(){
    VkVertexInputBindingDescription bindingDescription{};
    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof(Vertex);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    return bindingDescription;
};

VkVertexInputAttributeDescription* getAttributeDescriptions(uint32_t *attrDescriptionCount){
    VkVertexInputAttributeDescription* attrDescriptions = (VkVertexInputAttributeDescription*)malloc(sizeof(VkVertexInputAttributeDescription)*2);
    attrDescriptions[0].binding = 0;
    attrDescriptions[0].location = 0;
    attrDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
    attrDescriptions[0].offset = offsetof(Vertex, pos);
    attrDescriptions[1].binding = 0;
    attrDescriptions[1].location = 1;
    attrDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    attrDescriptions[1].offset = offsetof(Vertex, color);

    *attrDescriptionCount = 2;
    return attrDescriptions;
}


VkBuffer createVertexBuffer(VkDevice device, Vertex* vertices, uint32_t verticesSize){
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = sizeof(Vertex)*verticesSize;
    bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VkBuffer vertexBuffer;
    if(vkCreateBuffer(device, &bufferInfo, nullptr, &vertexBuffer) != VK_SUCCESS){
        printf("Failed to create Vertex Buffer!\n");
        exit(EXIT_FAILURE);
    }

    return vertexBuffer;
}

VkDeviceMemory allocateVertexBuffer(VkDevice device, VkPhysicalDevice physicalDevice, VkBuffer vertexBuffer){

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device, vertexBuffer, &memRequirements);
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(
        &memProperties, 
        memRequirements.memoryTypeBits, 
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    );

    VkDeviceMemory vertexBufferMemory;
    if(vkAllocateMemory(device, &allocInfo, nullptr, &vertexBufferMemory)!=VK_SUCCESS){
        printf("Failed to allocate Vertex Buffer Memory!\n");
        exit(EXIT_FAILURE);
    }

    vkBindBufferMemory(device, vertexBuffer, vertexBufferMemory, 0);//If the offset is non-zero, then it is required to be divisible by memRequirements.alignment.

    return vertexBufferMemory;
}