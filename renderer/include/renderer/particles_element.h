#ifndef _RENDERER_PARTICLES_ELEMENT_
#define _RENDERER_PARTICLES_ELEMENT_

#include <vector>
#include <renderer/scene_element.h>
#include <glm/glm.hpp>

namespace renderer {
    class ParticlesElement : public SceneElement
    {
    public:
        ParticlesElement(Device & device, VkRenderPass renderPass, std::size_t vbSize);
        virtual ~ParticlesElement();

        void setMVP(const glm::mat4& mvp);
        void setVertexBufferData(std::size_t count, void* data, std::size_t memSize);

        void recordToCmdBuffer(VkCommandBuffer commandBuffer);

    private:
        VkBuffer mvpUniform;
        VkDeviceSize mvpUniformSize;
        VkDeviceMemory mvpUniformDeviceMemory;

        VkDescriptorSetLayout descriptorSetLayout;
        VkPipelineLayout pipelineLayout;

        VkDescriptorPool descriptorPool;
        VkDescriptorSet mvpDescriptorSet;

        VkPipelineShaderStageCreateInfo vertexShaderStage;
        VkPipelineShaderStageCreateInfo fragmentShaderStage;

        VkBuffer vertexBuffer;
        VkDeviceMemory vertexBufferMemory;

        VkPipeline pipeline;

        std::size_t particleCount;

        VkViewport viewport;
        VkRect2D scissor;
    };
}

#endif
