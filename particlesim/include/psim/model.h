#ifndef _PSIM_MODEL_
#define _PSIM_MODEL_

#include <chrono>
#include <list>
#include <base/buffer.h>
#include <base/device.h>
#include <psim/base_generator.h>
#include <psim/interactor_defs.h>

namespace psim {
    class Model final
    {
    public:
        using size = base::vec::vec3f;
        using GeneratorList = std::list<generators::BaseGenerator*>;

        Model(base::Device& device, base::Buffer& buff, size wSize, const GeneratorList& genList, interactors::Setup i);
        Model(const Model & m) = delete;
        Model& operator = (const Model & m) = delete;
        ~Model();
        void progress(std::chrono::microseconds dt);
    private:

        union PushConstantData
        {
            struct 
            {
                uint32_t particleCount;
                float dt;
            } itemized;
            char data[sizeof(std::size_t) + sizeof(float)];
        };

        size worldSize;
        GeneratorList generators;

        VkDescriptorSetLayout descriptorSetLayout;
        VkDescriptorPool descriptorPool;
        VkDescriptorSet descriptorSet;
        VkPipelineShaderStageCreateInfo compShaderStage;
        VkPipelineLayout pipelineLayout;
        VkPipeline pipeline;

        VkCommandBuffer commandBuffer;
        VkCommandBufferBeginInfo bufferBeginInfo;
        VkFence fence;

        VkSubmitInfo submitInfo;

        base::Device& dev;
        base::Buffer& buffer;
		
        void toWorldScale(base::particle & p);
        void toNormalizedScale(base::particle & p);

        VkBuffer worldSizeUBO;
        VkDeviceMemory worldSizeUBODeviceMemory;

        VkBuffer interactorsUBO;
        VkDeviceMemory interactorsUBODeviceMemory;

        VkResult allocateBuffers();
    };
}

#endif
