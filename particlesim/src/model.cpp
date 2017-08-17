#include <psim/model.h>
#include <iostream>
#include <fstream>
#include <cmath>
#include <utility>

namespace psim {
    Model::Model(base::Device& device, base::Buffer& buff, size wSize, const GeneratorList & genList, interactors::Setup i)
        : worldSize(wSize)
        , dev(device)
        , buffer(buff)
    {
        VkResult result;

        for (const generators::BaseGenerator* g : genList) {
            generators::BaseGenerator *tmp = g->clone();
            tmp->setWorldSize(wSize);
            generators.push_back(tmp);
        }

        allocateBuffers();

        std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBinding(3); 
        descriptorSetLayoutBinding[0].binding = 0;
        descriptorSetLayoutBinding[0].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        descriptorSetLayoutBinding[0].descriptorCount = 1;
        descriptorSetLayoutBinding[0].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
        descriptorSetLayoutBinding[0].pImmutableSamplers = nullptr;

        descriptorSetLayoutBinding[1].binding = 1;
        descriptorSetLayoutBinding[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorSetLayoutBinding[1].descriptorCount = 1;
        descriptorSetLayoutBinding[1].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
        descriptorSetLayoutBinding[1].pImmutableSamplers = nullptr;

        descriptorSetLayoutBinding[2].binding = 2;
        descriptorSetLayoutBinding[2].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorSetLayoutBinding[2].descriptorCount = 1;
        descriptorSetLayoutBinding[2].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
        descriptorSetLayoutBinding[2].pImmutableSamplers = nullptr;

        VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCi;
        descriptorSetLayoutCi.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        descriptorSetLayoutCi.pNext = nullptr;
        descriptorSetLayoutCi.flags = 0;
        descriptorSetLayoutCi.bindingCount = 3;
        descriptorSetLayoutCi.pBindings = descriptorSetLayoutBinding.data();

        result = vkCreateDescriptorSetLayout(dev.device(), &descriptorSetLayoutCi, nullptr, &descriptorSetLayout);

        std::vector<VkDescriptorPoolSize> poolSize = { 
            {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1},
            {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 2} };

        VkDescriptorPoolCreateInfo descriptorPoolCi;
        descriptorPoolCi.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        descriptorPoolCi.pNext = nullptr;
        descriptorPoolCi.flags = 0;
        descriptorPoolCi.maxSets = 2;
        descriptorPoolCi.poolSizeCount = poolSize.size();
        descriptorPoolCi.pPoolSizes = poolSize.data();

        result = vkCreateDescriptorPool(dev.device(), &descriptorPoolCi, nullptr, &descriptorPool);

        VkDescriptorSetAllocateInfo descriptorSetAlloc;
        descriptorSetAlloc.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        descriptorSetAlloc.pNext = nullptr;
        descriptorSetAlloc.descriptorPool = descriptorPool;
        descriptorSetAlloc.descriptorSetCount = 1;
        descriptorSetAlloc.pSetLayouts = &descriptorSetLayout;

        result = vkAllocateDescriptorSets(dev.device(), &descriptorSetAlloc, &descriptorSet);

        VkDescriptorBufferInfo descriptorBufferInfo;
        descriptorBufferInfo.buffer = buffer.dataBuffer();
        descriptorBufferInfo.offset = 0;
        descriptorBufferInfo.range = buffer.dataSizeTotal();

        VkDescriptorBufferInfo worldSizeUBODescBufferInfo;
        worldSizeUBODescBufferInfo.buffer = worldSizeUBO;
        worldSizeUBODescBufferInfo.offset = 0;
        worldSizeUBODescBufferInfo.range = sizeof(size);

        VkDescriptorBufferInfo interactorsUBODescBufferInfo;
        interactorsUBODescBufferInfo.buffer = interactorsUBO;
        interactorsUBODescBufferInfo.offset = 0;
        interactorsUBODescBufferInfo.range = sizeof(interactors::Setup);


        std::vector<VkWriteDescriptorSet> wrt(3);
        wrt[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        wrt[0].pNext = nullptr;
        wrt[0].dstSet = descriptorSet;
        wrt[0].dstBinding = 0;
        wrt[0].dstArrayElement = 0;
        wrt[0].descriptorCount = 1;
        wrt[0].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        wrt[0].pBufferInfo = &descriptorBufferInfo;


        wrt[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        wrt[1].pNext = nullptr;
        wrt[1].dstSet = descriptorSet;
        wrt[1].dstBinding = 1;
        wrt[1].dstArrayElement = 0;
        wrt[1].descriptorCount = 1;
        wrt[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        wrt[1].pBufferInfo = &worldSizeUBODescBufferInfo;

        wrt[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        wrt[2].pNext = nullptr;
        wrt[2].dstSet = descriptorSet;
        wrt[2].dstBinding = 2;
        wrt[2].dstArrayElement = 0;
        wrt[2].descriptorCount = 1;
        wrt[2].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        wrt[2].pBufferInfo = &interactorsUBODescBufferInfo;

        vkUpdateDescriptorSets(dev.device(), wrt.size(), wrt.data(), 0, nullptr);

        std::ifstream cShaderStream("comp.spv", std::ios::ate | std::ios::binary);
        if (!cShaderStream.is_open()) {
            throw std::runtime_error("Could not open compute shader file");
        }

        std::vector<char> computeShaderData;
        auto cShaderSize = cShaderStream.tellg();
        computeShaderData.resize(cShaderSize);
        cShaderStream.seekg(std::ios::beg);
        cShaderStream.read(computeShaderData.data(), cShaderSize);
        cShaderStream.close();

        VkShaderModuleCreateInfo computeShaderModuleCi;
        computeShaderModuleCi.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        computeShaderModuleCi.pNext = nullptr;
        computeShaderModuleCi.flags = 0;
        computeShaderModuleCi.codeSize = cShaderSize;
        computeShaderModuleCi.pCode = reinterpret_cast<uint32_t*>(computeShaderData.data());

        compShaderStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        compShaderStage.pNext = nullptr;
        compShaderStage.flags = 0;
        compShaderStage.stage = VK_SHADER_STAGE_COMPUTE_BIT;
        compShaderStage.pName = "main";
        compShaderStage.pSpecializationInfo = nullptr;

        result = vkCreateShaderModule(dev.device(), &computeShaderModuleCi, nullptr, &compShaderStage.module);

        VkPushConstantRange pushConstants;
        pushConstants.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
        pushConstants.offset = 0;
        pushConstants.size = sizeof(PushConstantData::data) ;

        VkPipelineLayoutCreateInfo pipelineLayoutCi;
        pipelineLayoutCi.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutCi.pNext = nullptr;
        pipelineLayoutCi.flags = 0;
        pipelineLayoutCi.setLayoutCount = 1;
        pipelineLayoutCi.pSetLayouts = &descriptorSetLayout;
        pipelineLayoutCi.pushConstantRangeCount = 1;
        pipelineLayoutCi.pPushConstantRanges = &pushConstants;

        result = vkCreatePipelineLayout(dev.device(), &pipelineLayoutCi, nullptr, &pipelineLayout);

        VkComputePipelineCreateInfo compPipelineCi;
        compPipelineCi.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
        compPipelineCi.pNext = nullptr;
        compPipelineCi.flags = 0;
        compPipelineCi.stage = compShaderStage;
        compPipelineCi.layout = pipelineLayout;
        compPipelineCi.basePipelineHandle = VK_NULL_HANDLE;
        compPipelineCi.basePipelineIndex = 0;

        vkCreateComputePipelines(dev.device(), nullptr, 1, &compPipelineCi, nullptr, &pipeline);

        VkCommandBufferAllocateInfo commandBufferCi;
        commandBufferCi.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        commandBufferCi.pNext = nullptr;
        commandBufferCi.commandPool = dev.commandPool();
        commandBufferCi.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        commandBufferCi.commandBufferCount = 1;

        result = vkAllocateCommandBuffers(dev.device(), &commandBufferCi, &commandBuffer);

        bufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        bufferBeginInfo.pNext = nullptr;
        bufferBeginInfo.flags = 0;
        bufferBeginInfo.pInheritanceInfo = nullptr;

        VkFenceCreateInfo fenceCi;
        fenceCi.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceCi.pNext = nullptr;
        fenceCi.flags = 0;
        
        result = vkCreateFence(dev.device(), &fenceCi, nullptr, &fence);

        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.pNext = nullptr;
        submitInfo.waitSemaphoreCount = 0;
        submitInfo.pWaitSemaphores = nullptr;
        submitInfo.pWaitDstStageMask = nullptr;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;
        submitInfo.signalSemaphoreCount = 0;
        submitInfo.pSignalSemaphores = nullptr;

        void* mmap;
        vkMapMemory(dev.device(), worldSizeUBODeviceMemory, 0, sizeof(size), 0, &mmap);
        memcpy(mmap, &wSize, sizeof(size));
        vkUnmapMemory(dev.device(), worldSizeUBODeviceMemory);

        vkMapMemory(dev.device(), interactorsUBODeviceMemory, 0, sizeof(interactors::Setup), 0, &mmap);
        memcpy(mmap, &i, sizeof(interactors::Setup));
        vkUnmapMemory(dev.device(), interactorsUBODeviceMemory);
    }

    Model::~Model()
    {

        vkDestroyBuffer(dev.device(), interactorsUBO, nullptr);
        vkFreeMemory(dev.device(), interactorsUBODeviceMemory, nullptr);
        vkDestroyBuffer(dev.device(), worldSizeUBO, nullptr);
        vkFreeMemory(dev.device(), worldSizeUBODeviceMemory, nullptr);
        vkDestroyFence(dev.device(), fence, nullptr);
        vkDestroyPipeline(dev.device(), pipeline, nullptr);
        vkDestroyPipelineLayout(dev.device(), pipelineLayout, nullptr);
        vkDestroyShaderModule(dev.device(), compShaderStage.module, nullptr);
        vkDestroyDescriptorSetLayout(dev.device(), descriptorSetLayout, nullptr);
        vkDestroyDescriptorPool(dev.device(), descriptorPool, nullptr); 
        for (generators::BaseGenerator* g : generators) {
            delete g;
            g = nullptr;
        }
        generators.clear();
    }

    void Model::progress(std::chrono::microseconds dt)
    {
        //1 time
        base::scalar::time dt_sec = dt.count() *1.0e-6f;
        //2 interactors
        std::size_t particleCount = buffer.activeCount();
        if(particleCount > 0) {
            VkResult result;

            result = vkResetFences(dev.device(), 1, &fence);
            result = vkBeginCommandBuffer(commandBuffer, &bufferBeginInfo);

            vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline);
            vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);
            PushConstantData pc;
            pc.itemized.particleCount = particleCount;
            pc.itemized.dt = dt_sec;

            vkCmdPushConstants(commandBuffer, pipelineLayout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(pc.data), pc.data);

            uint32_t numCompGroups = ceil(particleCount/128);
            vkCmdDispatch(commandBuffer, numCompGroups, 1, 1);
            vkEndCommandBuffer(commandBuffer);

            result = vkQueueSubmit(dev.computeQueue(), 1, &submitInfo, fence);

            VkResult waitResult;
            do {
                waitResult = vkWaitForFences(dev.device(), 1, &fence, VK_TRUE, UINT64_MAX);
            } while (waitResult == VK_TIMEOUT);
        }

        //3 sort
        buffer.mapToHost();
        buffer.sort();
        //4 generators
        for (generators::BaseGenerator* g : generators) {
            g->generate(buffer, dt_sec);
        }

        buffer.unmapFromHost();
    }

    void Model::toWorldScale(base::particle & p)
    {
        p.pos = { p.pos[base::axis::X] * worldSize[base::axis::X],
                p.pos[base::axis::Y] * worldSize[base::axis::Y],
                p.pos[base::axis::Z] * worldSize[base::axis::Z] };
    }

    void Model::toNormalizedScale(base::particle & p)
    {
        p.pos = { p.pos[base::axis::X] / worldSize[base::axis::X],
                p.pos[base::axis::Y] / worldSize[base::axis::Y],
                p.pos[base::axis::Z] / worldSize[base::axis::Z] };
    }

    VkResult Model::allocateBuffers()
    {
        VkResult result;

        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(dev.physicalDevice(), &memProperties);

        auto findMemIndex = [&memProperties] (VkMemoryRequirements memReqs) -> std::pair<bool, uint32_t>
        {
            uint32_t memTypeIndex;
            bool memoryTypeFound = false;
            auto memTags = (VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
            for (memTypeIndex = 0; memTypeIndex < memProperties.memoryTypeCount; ++memTypeIndex) {
                if ((memReqs.memoryTypeBits & (1<< memTypeIndex)) &&
                   ((memProperties.memoryTypes[memTypeIndex].propertyFlags & memTags) == memTags)) {
                        memoryTypeFound = true;
                        break;
                }
            }

            return std::make_pair(memoryTypeFound, memTypeIndex);
        };

        VkBufferCreateInfo worldSizeUBOCi;
        worldSizeUBOCi.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        worldSizeUBOCi.pNext = nullptr;
        worldSizeUBOCi.flags = 0;
        worldSizeUBOCi.size = sizeof(size);
        worldSizeUBOCi.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        worldSizeUBOCi.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        worldSizeUBOCi.queueFamilyIndexCount = 0;
        worldSizeUBOCi.pQueueFamilyIndices = nullptr;

        result = vkCreateBuffer(dev.device(), &worldSizeUBOCi, nullptr, &worldSizeUBO);

        VkMemoryRequirements memoryRequirements;
        vkGetBufferMemoryRequirements(dev.device(), worldSizeUBO, &memoryRequirements);

        auto memIdxSearch = findMemIndex(memoryRequirements);

        if (!memIdxSearch.first) {
            throw std::runtime_error("Could not find proper memory type for uniform buffer");
        }

        VkMemoryAllocateInfo worldSizeUBOAlloc;
        worldSizeUBOAlloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        worldSizeUBOAlloc.pNext = nullptr;
        worldSizeUBOAlloc.memoryTypeIndex = memIdxSearch.second;
        worldSizeUBOAlloc.allocationSize = memoryRequirements.size;

        result = vkAllocateMemory(dev.device(), &worldSizeUBOAlloc, nullptr, &worldSizeUBODeviceMemory);
        result = vkBindBufferMemory(dev.device(), worldSizeUBO, worldSizeUBODeviceMemory, 0);

        VkBufferCreateInfo interactorsUBOCi;
        interactorsUBOCi.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        interactorsUBOCi.pNext = nullptr;
        interactorsUBOCi.flags = 0;
        interactorsUBOCi.size = sizeof(interactors::Setup);
        interactorsUBOCi.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        interactorsUBOCi.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        interactorsUBOCi.queueFamilyIndexCount = 0;
        interactorsUBOCi.pQueueFamilyIndices = nullptr;

        result = vkCreateBuffer(dev.device(), &interactorsUBOCi, nullptr, &interactorsUBO);

        vkGetBufferMemoryRequirements(dev.device(), interactorsUBO, &memoryRequirements);
        
        memIdxSearch = findMemIndex(memoryRequirements);

        if (!memIdxSearch.first) {
            throw std::runtime_error("Could not find proper memory type for uniform buffer");
        }

        VkMemoryAllocateInfo interactorsUBOAlloc;
        interactorsUBOAlloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        interactorsUBOAlloc.pNext = nullptr;
        interactorsUBOAlloc.memoryTypeIndex = memIdxSearch.second;
        interactorsUBOAlloc.allocationSize = memoryRequirements.size;

        result = vkAllocateMemory(dev.device(), &interactorsUBOAlloc, nullptr, &interactorsUBODeviceMemory);
        result = vkBindBufferMemory(dev.device(), interactorsUBO, interactorsUBODeviceMemory, 0);

		return VK_SUCCESS;
    }
}

