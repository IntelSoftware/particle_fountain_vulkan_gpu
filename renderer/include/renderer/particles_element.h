#ifndef _RENDERER_PARTICLES_ELEMENT_
#define _RENDERER_PARTICLES_ELEMENT_

#include <vector>
#include <renderer/scene_element.h>
#include <glm/glm.hpp>

namespace renderer {
    class ParticlesElement : public SceneElement
    {
    public:
        ParticlesElement(base::Device & device, VkRenderPass renderPass, std::size_t vbSize);
        virtual ~ParticlesElement();

        void setMVP(const glm::mat4& mvp);
        void setVertexBuffer(VkBuffer vb) noexcept; 
        void setCurrentCount(std::size_t count) noexcept;

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

        VkPipeline pipeline;

        VkBuffer vertexBuffer;
        std::size_t particleCount;

        VkViewport viewport;
        VkRect2D scissor;
    };
}

#endif
