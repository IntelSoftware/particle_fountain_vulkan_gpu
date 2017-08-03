#ifndef _RENDERER_SCENE_
#define _RENDERER_SCENE_

#include <vector>
#include <renderer/device.h>
#include <renderer/scene_element.h>

namespace renderer {
    class Scene
    {
    public:
        enum ATTACHMENT {
                COLOR = 0x0,
                DEPTH = 0x1
        };

        Scene(Device& device);
        ~Scene();
        void addToScene(SceneElement *element);

        void render();

        Device& rendererDevice() noexcept;
        VkRenderPass renderPass() noexcept;

    private:
        std::vector<SceneElement*> objectsList;

        Device& rDevice;

        VkRenderPass rPass;
        std::vector<VkFramebuffer> framebuffers;
        VkCommandBuffer commandBuffer;

        VkCommandBufferBeginInfo bufferBeginInfo;
        VkClearValue clear_values[2];
        VkRenderPassBeginInfo renderPassBegin;

        VkSemaphore swapchainImageSemaphore;
        VkFence fence;
        VkPipelineStageFlags pipelineFlags;
        VkSubmitInfo submitInfo;
    };
}

#endif // !_RENDERER_SCENE_
