#include "vertex.h"
#include <cstdio>
#include <cstring>
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


void createVertexBuffer(
    VkDevice device, 
    VkPhysicalDevice physicalDevice,
    VkCommandPool commandPool,
    VkQueue transferQueue,
    Vertex* vertices, 
    uint32_t verticesSize,
    VkBuffer *vertexBuffer,
    VkDeviceMemory *vertexBufferMemory
){
    VkDeviceSize bufferSize = sizeof(Vertex)*verticesSize;

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    createBuffer(
        device,
        physicalDevice,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        bufferSize,
        &stagingBuffer,
        &stagingBufferMemory
    );

    void* data;
    vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, vertices, bufferSize);
    vkUnmapMemory(device, stagingBufferMemory);

    createBuffer(
        device, 
        physicalDevice,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        bufferSize,
        vertexBuffer,
        vertexBufferMemory
    );

    copyBuffer(
        device,
        commandPool,
        transferQueue,
        stagingBuffer,
        *vertexBuffer,
        bufferSize
    );

    vkDestroyBuffer(device, stagingBuffer, nullptr);
    vkFreeMemory(device, stagingBufferMemory, nullptr);
}

void createIndexBuffer(
    VkDevice device, 
    VkPhysicalDevice physicalDevice,
    VkCommandPool transientCommandPool,
    VkQueue transferQueue,
    uint32_t* indices, 
    uint32_t indicesSize,
    VkBuffer *indexBuffer,
    VkDeviceMemory *indexBufferMemory
){
    VkDeviceSize bufferSize = sizeof(uint32_t)*indicesSize;

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    createBuffer(
        device,
        physicalDevice,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        bufferSize,
        &stagingBuffer,
        &stagingBufferMemory
    );

    void* data;
    vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, indices, bufferSize);
    vkUnmapMemory(device, stagingBufferMemory);

    createBuffer(
        device, 
        physicalDevice,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        bufferSize,
        indexBuffer,
        indexBufferMemory
    );

    copyBuffer(
        device,
        transientCommandPool,
        transferQueue,
        stagingBuffer,
        *indexBuffer,
        bufferSize
    );

    vkDestroyBuffer(device, stagingBuffer, nullptr);
    vkFreeMemory(device, stagingBufferMemory, nullptr);
}