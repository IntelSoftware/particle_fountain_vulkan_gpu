#include <renderer/particles_element.h>
#include <fstream>
#include <iostream>

#include <psim/particle.h>

namespace renderer {
    ParticlesElement::ParticlesElement(Device & device, VkRenderPass renderPass, std::size_t vbSize)
        :SceneElement(device)
        , mvpUniformSize(sizeof(glm::mat4))
        , particleCount(0)
    {
        VkResult result;
        VkBufferCreateInfo mvpUniformCi;
        mvpUniformCi.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        mvpUniformCi.pNext = nullptr;
        mvpUniformCi.flags = 0;
        mvpUniformCi.size = mvpUniformSize;
        mvpUniformCi.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        mvpUniformCi.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        mvpUniformCi.queueFamilyIndexCount = 0;
        mvpUniformCi.pQueueFamilyIndices = nullptr;

        result = vkCreateBuffer(renderDevice.device(), &mvpUniformCi, nullptr, &mvpUniform);

        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(renderDevice.physicalDevice(), &memProperties);

        VkMemoryRequirements mvpUniformMemoryRequirements;
        vkGetBufferMemoryRequirements(renderDevice.device(), mvpUniform, &mvpUniformMemoryRequirements);

        uint32_t memTypeIndex;
        bool memoryTypeFound = false;
        auto memTags = (VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        for (memTypeIndex = 0; memTypeIndex < memProperties.memoryTypeCount; ++memTypeIndex) {
            if ((mvpUniformMemoryRequirements.memoryTypeBits & (1<< memTypeIndex)) &&
               ((memProperties.memoryTypes[memTypeIndex].propertyFlags & memTags) == memTags)) {
                    memoryTypeFound = true;
                    break;
            }
        }

        if (!memoryTypeFound) {
            throw std::runtime_error("Could not find proper memory type for uniform buffer");
        }

        VkMemoryAllocateInfo mvpUniformAlloc;
        mvpUniformAlloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        mvpUniformAlloc.pNext = nullptr;
        mvpUniformAlloc.memoryTypeIndex = memTypeIndex;
        mvpUniformAlloc.allocationSize = mvpUniformMemoryRequirements.size;

        result = vkAllocateMemory(renderDevice.device(), &mvpUniformAlloc, nullptr, &mvpUniformDeviceMemory);
        result = vkBindBufferMemory(renderDevice.device(), mvpUniform, mvpUniformDeviceMemory, 0);

        VkDescriptorSetLayoutBinding descriptorSetLayoutBinding;
        descriptorSetLayoutBinding.binding = 0;
        descriptorSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorSetLayoutBinding.descriptorCount = 1;
        descriptorSetLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        descriptorSetLayoutBinding.pImmutableSamplers = nullptr;

        VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCi;
        descriptorSetLayoutCi.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        descriptorSetLayoutCi.pNext = nullptr;
        descriptorSetLayoutCi.flags = 0;
        descriptorSetLayoutCi.bindingCount = 1;
        descriptorSetLayoutCi.pBindings = &descriptorSetLayoutBinding;

        result = vkCreateDescriptorSetLayout(renderDevice.device(), &descriptorSetLayoutCi, nullptr, &descriptorSetLayout);

        VkPipelineLayoutCreateInfo pipelineLayoutCi;
        pipelineLayoutCi.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutCi.pNext = nullptr;
        pipelineLayoutCi.flags = 0;
        pipelineLayoutCi.setLayoutCount = 1;
        pipelineLayoutCi.pSetLayouts = &descriptorSetLayout;
        pipelineLayoutCi.pushConstantRangeCount = 0;
        pipelineLayoutCi.pPushConstantRanges = nullptr;

        result = vkCreatePipelineLayout(renderDevice.device(), &pipelineLayoutCi, nullptr, &pipelineLayout);

        VkDescriptorPoolSize poolSize { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1 };
        VkDescriptorPoolCreateInfo descriptorPoolCi;
        descriptorPoolCi.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        descriptorPoolCi.pNext = nullptr;
        descriptorPoolCi.flags = 0;
        descriptorPoolCi.maxSets = 1;
        descriptorPoolCi.poolSizeCount = 1;
        descriptorPoolCi.pPoolSizes = &poolSize;

        result = vkCreateDescriptorPool(renderDevice.device(), &descriptorPoolCi, nullptr, &descriptorPool);

        VkDescriptorSetAllocateInfo descriptorSetAlloc;
        descriptorSetAlloc.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        descriptorSetAlloc.pNext = nullptr;
        descriptorSetAlloc.descriptorPool = descriptorPool;
        descriptorSetAlloc.descriptorSetCount = 1;
        descriptorSetAlloc.pSetLayouts = &descriptorSetLayout;

        result = vkAllocateDescriptorSets(renderDevice.device(), &descriptorSetAlloc, &mvpDescriptorSet);

        VkDescriptorBufferInfo mvpUniformDescriptorBufferInfo;
        mvpUniformDescriptorBufferInfo.buffer = mvpUniform;
        mvpUniformDescriptorBufferInfo.offset = 0;
        mvpUniformDescriptorBufferInfo.range = mvpUniformSize;

        VkWriteDescriptorSet wrt;
        wrt.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        wrt.pNext = nullptr;
        wrt.dstSet = mvpDescriptorSet;
        wrt.dstBinding = 0;
        wrt.dstArrayElement = 0;
        wrt.descriptorCount = 1;
        wrt.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        wrt.pBufferInfo = &mvpUniformDescriptorBufferInfo;

        vkUpdateDescriptorSets(renderDevice.device(), 1, &wrt, 0, nullptr);
        
        std::ifstream vShaderStream("vert.spv", std::ios::ate | std::ios::binary);
        if (!vShaderStream.is_open()) {
            throw std::runtime_error("Could not open vertex shader file");
        }

        std::ifstream fShaderStream("frag.spv", std::ios::ate | std::ios::binary);
        if (!fShaderStream.is_open()) {
            throw std::runtime_error("Could not open fragment shader file");
        }

        std::vector<char> vertexShaderData;
        std::vector<char> fragmentShaderData;
        auto vShaderSize = vShaderStream.tellg();
        auto fShaderSize = fShaderStream.tellg();
        vertexShaderData.resize(vShaderSize);
        fragmentShaderData.resize(fShaderSize);

        vShaderStream.seekg(std::ios::beg);
        fShaderStream.seekg(std::ios::beg);

        vShaderStream.read(vertexShaderData.data(), vShaderSize);
        fShaderStream.read(fragmentShaderData.data(), fShaderSize);

        vShaderStream.close();
        fShaderStream.close();
        
        VkShaderModuleCreateInfo vertexShaderModuleCi;
        vertexShaderModuleCi.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        vertexShaderModuleCi.pNext = nullptr;
        vertexShaderModuleCi.flags = 0;
        vertexShaderModuleCi.codeSize = vShaderSize;
        vertexShaderModuleCi.pCode = reinterpret_cast<uint32_t*>(vertexShaderData.data());

        vertexShaderStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertexShaderStage.pNext = nullptr;
        vertexShaderStage.flags = 0;
        vertexShaderStage.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertexShaderStage.pName = "main";
        vertexShaderStage.pSpecializationInfo = nullptr;

        result = vkCreateShaderModule(renderDevice.device(), &vertexShaderModuleCi, nullptr, &vertexShaderStage.module);
        
        VkShaderModuleCreateInfo fragmentShaderModuleCi;
        fragmentShaderModuleCi.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        fragmentShaderModuleCi.pNext = nullptr;
        fragmentShaderModuleCi.flags = 0;
        fragmentShaderModuleCi.codeSize = fShaderSize;
        fragmentShaderModuleCi.pCode = reinterpret_cast<uint32_t*>(fragmentShaderData.data());

        fragmentShaderStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragmentShaderStage.pNext = nullptr;
        fragmentShaderStage.flags = 0;
        fragmentShaderStage.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragmentShaderStage.pName = "main";
        fragmentShaderStage.pSpecializationInfo = nullptr;

        result = vkCreateShaderModule(renderDevice.device(), &fragmentShaderModuleCi, nullptr, &fragmentShaderStage.module);

        VkBufferCreateInfo vbCi;
        vbCi.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        vbCi.pNext = nullptr;
        vbCi.flags = 0;
        vbCi.size = vbSize;
        vbCi.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        vbCi.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        vbCi.queueFamilyIndexCount = 0;
        vbCi.pQueueFamilyIndices = nullptr;

        result = vkCreateBuffer(renderDevice.device(), &vbCi, nullptr, &vertexBuffer);

        VkMemoryRequirements vbMemoryRequirements;
        vkGetBufferMemoryRequirements(renderDevice.device(), vertexBuffer, &vbMemoryRequirements);

        memoryTypeFound = false;
        memTags = (VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        for (memTypeIndex = 0; memTypeIndex < memProperties.memoryTypeCount; ++memTypeIndex) {
            if ((vbMemoryRequirements.memoryTypeBits & (1<< memTypeIndex)) &&
               ((memProperties.memoryTypes[memTypeIndex].propertyFlags & memTags) == memTags)) {
                memoryTypeFound = true;
                break;
            }
        }

        if (!memoryTypeFound) {
            throw std::runtime_error("Could not find proper memory type for vertex buffer");
        }

        VkMemoryAllocateInfo vbAlloc;
        vbAlloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        vbAlloc.pNext = nullptr;
        vbAlloc.memoryTypeIndex = memTypeIndex;
        vbAlloc.allocationSize = vbMemoryRequirements.size;

        result = vkAllocateMemory(renderDevice.device(), &vbAlloc, nullptr, &vertexBufferMemory);
        result = vkBindBufferMemory(renderDevice.device(), vertexBuffer, vertexBufferMemory, 0);

        VkVertexInputBindingDescription vertexBinding;

        vertexBinding.binding = 0;
        vertexBinding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        vertexBinding.stride = sizeof(psim::particle);
        
        VkVertexInputAttributeDescription vertexAttribute;

        vertexAttribute.binding = 0;
        vertexAttribute.format = VK_FORMAT_R32G32B32A32_SFLOAT;
        vertexAttribute.location = 0;
        vertexAttribute.offset = 0;
        

        //-----------PIPELINE-------------------
        std::vector<VkDynamicState> dynamicStates(VK_DYNAMIC_STATE_RANGE_SIZE);
        dynamicStates[0] = VK_DYNAMIC_STATE_VIEWPORT;
        dynamicStates[1] = VK_DYNAMIC_STATE_SCISSOR;

        VkPipelineDynamicStateCreateInfo dynamicStateCi;
        dynamicStateCi.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicStateCi.pNext = nullptr;
        dynamicStateCi.flags = 0;
        dynamicStateCi.dynamicStateCount = 2;
        dynamicStateCi.pDynamicStates = dynamicStates.data();

        VkPipelineVertexInputStateCreateInfo vertexInputStateCi;
        vertexInputStateCi.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputStateCi.pNext = nullptr;
        vertexInputStateCi.flags = 0;
        vertexInputStateCi.vertexBindingDescriptionCount = 1;
        vertexInputStateCi.pVertexBindingDescriptions = &vertexBinding;
        vertexInputStateCi.vertexAttributeDescriptionCount = 1;
        vertexInputStateCi.pVertexAttributeDescriptions = &vertexAttribute;
        
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyCi;
        inputAssemblyCi.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssemblyCi.pNext = nullptr;
        inputAssemblyCi.flags = 0;
        inputAssemblyCi.topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
        inputAssemblyCi.primitiveRestartEnable = VK_FALSE;

        VkPipelineRasterizationStateCreateInfo rastStateCi;
        rastStateCi.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rastStateCi.pNext = nullptr;
        rastStateCi.flags = 0;
        rastStateCi.polygonMode = VK_POLYGON_MODE_FILL;
        rastStateCi.cullMode = VK_CULL_MODE_BACK_BIT;
        rastStateCi.frontFace = VK_FRONT_FACE_CLOCKWISE;
        rastStateCi.depthClampEnable = VK_FALSE;
        rastStateCi.rasterizerDiscardEnable = VK_FALSE;
        rastStateCi.depthBiasEnable = VK_FALSE;
        rastStateCi.depthBiasConstantFactor = 0;
        rastStateCi.depthBiasClamp = 0;
        rastStateCi.depthBiasSlopeFactor = 0;
        rastStateCi.lineWidth = 1.0f;

        VkPipelineColorBlendAttachmentState colorBlendAttachment;
        colorBlendAttachment.colorWriteMask = 0xF;
        colorBlendAttachment.blendEnable = VK_FALSE;
        colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
        colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
        colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ZERO;
        colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
        colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;

        VkPipelineColorBlendStateCreateInfo colorBlendCi;
        colorBlendCi.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlendCi.pNext = nullptr;
        colorBlendCi.flags = 0;
        colorBlendCi.attachmentCount = 1;
        colorBlendCi.pAttachments = &colorBlendAttachment;
        colorBlendCi.logicOpEnable = VK_FALSE;
        colorBlendCi.logicOp = VK_LOGIC_OP_NO_OP;
        colorBlendCi.blendConstants[0] = 1.0f;
        colorBlendCi.blendConstants[1] = 1.0f;
        colorBlendCi.blendConstants[2] = 1.0f;
        colorBlendCi.blendConstants[3] = 1.0f;
        
        VkPipelineViewportStateCreateInfo viewportStateCi;
        viewportStateCi.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportStateCi.pNext = nullptr;
        viewportStateCi.flags = 0;
        viewportStateCi.viewportCount = 1;
        viewportStateCi.pViewports = nullptr;
        viewportStateCi.scissorCount = 1;
        viewportStateCi.pScissors = nullptr;

        VkPipelineDepthStencilStateCreateInfo depthStencilCi;
        depthStencilCi.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depthStencilCi.pNext = nullptr;
        depthStencilCi.flags = 0;
        depthStencilCi.depthTestEnable = VK_TRUE;
        depthStencilCi.depthWriteEnable = VK_TRUE;
        depthStencilCi.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
        depthStencilCi.depthBoundsTestEnable = VK_FALSE;
        depthStencilCi.minDepthBounds = 0;
        depthStencilCi.maxDepthBounds = 0;
        depthStencilCi.stencilTestEnable = VK_FALSE;
        depthStencilCi.back.failOp = VK_STENCIL_OP_KEEP;
        depthStencilCi.back.passOp = VK_STENCIL_OP_KEEP;
        depthStencilCi.back.compareOp = VK_COMPARE_OP_ALWAYS;
        depthStencilCi.back.compareMask = 0;
        depthStencilCi.back.reference = 0;
        depthStencilCi.back.depthFailOp = VK_STENCIL_OP_KEEP;
        depthStencilCi.back.writeMask = 0;
        depthStencilCi.front = depthStencilCi.back;

        VkPipelineMultisampleStateCreateInfo multisampleCi;
        multisampleCi.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampleCi.pNext = nullptr;
        multisampleCi.flags = 0;
        multisampleCi.pSampleMask = nullptr;
        multisampleCi.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        multisampleCi.sampleShadingEnable = VK_FALSE;
        multisampleCi.alphaToCoverageEnable = VK_FALSE;
        multisampleCi.alphaToOneEnable = VK_FALSE;
        multisampleCi.minSampleShading = 0.0;

        VkPipelineShaderStageCreateInfo shaderStages[2] = { vertexShaderStage, fragmentShaderStage };
        VkGraphicsPipelineCreateInfo pipelineCi;
        pipelineCi.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineCi.pNext = nullptr;
        pipelineCi.flags = 0;
        pipelineCi.layout = pipelineLayout;
        pipelineCi.basePipelineHandle = VK_NULL_HANDLE;
        pipelineCi.basePipelineIndex = 0;
        pipelineCi.pVertexInputState = &vertexInputStateCi;
        pipelineCi.pInputAssemblyState = &inputAssemblyCi;
        pipelineCi.pRasterizationState = &rastStateCi;
        pipelineCi.pColorBlendState = &colorBlendCi;
        pipelineCi.pTessellationState = nullptr;
        pipelineCi.pMultisampleState = &multisampleCi;
        pipelineCi.pDynamicState = &dynamicStateCi;
        pipelineCi.pViewportState = &viewportStateCi;
        pipelineCi.pDepthStencilState = &depthStencilCi;
        pipelineCi.pStages = shaderStages;
        pipelineCi.stageCount = 2;
        pipelineCi.renderPass = renderPass;
        pipelineCi.subpass = 0;

        result = vkCreateGraphicsPipelines(renderDevice.device(), nullptr, 1, &pipelineCi, nullptr, &pipeline);

        //mid render elements partially completed during recording buffer

        viewport.x = 0;
        viewport.y = 0;
        viewport.width = renderDevice.windowSize().width;
        viewport.height = renderDevice.windowSize().height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        scissor.offset = { 0, 0 };
        scissor.extent = renderDevice.windowSize();

    }
        
    ParticlesElement::~ParticlesElement()
    {
        vkDestroyPipeline(renderDevice.device(), pipeline, nullptr);
        vkDestroyBuffer(renderDevice.device(), vertexBuffer, nullptr);
        vkFreeMemory(renderDevice.device(), vertexBufferMemory, nullptr);
        vkDestroyShaderModule(renderDevice.device(), vertexShaderStage.module, nullptr);
        vkDestroyShaderModule(renderDevice.device(), fragmentShaderStage.module, nullptr);
        vkDestroyDescriptorPool(renderDevice.device(), descriptorPool, nullptr);
        vkDestroyPipelineLayout(renderDevice.device(), pipelineLayout, nullptr);
        vkDestroyDescriptorSetLayout(renderDevice.device(), descriptorSetLayout, nullptr);
        vkDestroyBuffer(renderDevice.device(), mvpUniform, nullptr);
        vkFreeMemory(renderDevice.device(), mvpUniformDeviceMemory, nullptr);
    }

    void ParticlesElement::setMVP(const glm::mat4 & mvp)
    {
        void *mappedData;

        vkMapMemory(renderDevice.device(), mvpUniformDeviceMemory, 0, mvpUniformSize, 0, &mappedData);
        memcpy(mappedData, &mvp, mvpUniformSize);
        vkUnmapMemory(renderDevice.device(), mvpUniformDeviceMemory);
    }

    void ParticlesElement::setVertexBufferData(std::size_t count, void * data, std::size_t memSize)
    {
        particleCount = count;
        void *mappedData;

        vkMapMemory(renderDevice.device(), vertexBufferMemory, 0, memSize, 0, &mappedData);
        memcpy(mappedData, data, memSize);
        vkUnmapMemory(renderDevice.device(), vertexBufferMemory);
    }

    void ParticlesElement::recordToCmdBuffer(VkCommandBuffer commandBuffer)
    {
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &mvpDescriptorSet, 0, nullptr);

        VkDeviceSize offset = 0;
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vertexBuffer, &offset);
        
        viewport.width = renderDevice.windowSize().width;
        viewport.height = renderDevice.windowSize().height;
        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

        scissor.extent = renderDevice.windowSize();
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

        vkCmdDraw(commandBuffer, particleCount, 1, 0, 0);
    }
}
