#include "descriptor.h"
#include <cstdio>
#include <cstdlib>
#include "vk.h"

VkDescriptorSetLayout createDescriptorSetLayout(VkDevice device){
    VkDescriptorSetLayoutBinding uboLayoutBinding{};
    uboLayoutBinding.binding = 0;
    uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboLayoutBinding.descriptorCount = 1;
    uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    uboLayoutBinding.pImmutableSamplers = nullptr;

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = 1;
    layoutInfo.pBindings = &uboLayoutBinding;

    VkDescriptorSetLayout descriptorSetLayout;
    if(vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS){
        printf("Failed to create Descriptor Set Layout!\n");
        exit(EXIT_FAILURE);
    }

    return descriptorSetLayout;
}

void createUniformBuffers(
    VkDevice device,
    VkPhysicalDeviceMemoryProperties* memProperties,
    uint32_t uniformBufferCount, 
    VkBuffer *uniformBuffers, 
    VkDeviceMemory *uniformBuffersMemory,
    void* *uniformBuffersMapped
){
    VkDeviceSize bufferSize = sizeof(UniformBufferObject);

    for(int i = 0; i < uniformBufferCount; i++){
        createBuffer(
            device,
            memProperties,
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            bufferSize,
            &uniformBuffers[i],
            &uniformBuffersMemory[i]
        );

        vkMapMemory(device, uniformBuffersMemory[i], 0, bufferSize, 0, &uniformBuffersMapped[i]);
    }
}