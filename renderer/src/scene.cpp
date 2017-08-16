#include <renderer/scene.h>

namespace renderer {
    Scene::Scene(base::Device & device)
        : rDevice(device)
    {
        VkResult result; 

        VkAttachmentDescription attachments[2];
        
        attachments[COLOR].format = rDevice.swapchainSurfaceFormat().format;
        attachments[COLOR].flags = 0;
        attachments[COLOR].samples = VK_SAMPLE_COUNT_1_BIT;
        attachments[COLOR].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        attachments[COLOR].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        attachments[COLOR].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        attachments[COLOR].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        attachments[COLOR].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        attachments[COLOR].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        attachments[DEPTH].format = rDevice.depthBufferFormat();
        attachments[DEPTH].flags = 0;
        attachments[DEPTH].samples = VK_SAMPLE_COUNT_1_BIT;
        attachments[DEPTH].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        attachments[DEPTH].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        attachments[DEPTH].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        attachments[DEPTH].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        attachments[DEPTH].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        attachments[DEPTH].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentReference refColor;
        refColor.attachment = 0;
        refColor.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkAttachmentReference refDepth;
        refDepth.attachment = 1;
        refDepth.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass;
        subpass.flags = 0;
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.inputAttachmentCount = 0;
        subpass.pInputAttachments = nullptr;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &refColor;
        subpass.pResolveAttachments = nullptr;
        subpass.pDepthStencilAttachment = &refDepth;
        subpass.preserveAttachmentCount = 0;
        subpass.pPreserveAttachments = nullptr;

        VkRenderPassCreateInfo renderpassCi;
        renderpassCi.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderpassCi.pNext = nullptr;
        renderpassCi.flags = 0;
        renderpassCi.attachmentCount = 2;
        renderpassCi.pAttachments = attachments;
        renderpassCi.subpassCount = 1;
        renderpassCi.pSubpasses = &subpass;
        renderpassCi.dependencyCount = 0;
        renderpassCi.pDependencies = nullptr;

        result = vkCreateRenderPass(rDevice.device(), &renderpassCi, nullptr, &rPass);

        VkFramebufferCreateInfo framebufferCi;
        framebufferCi.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferCi.pNext = nullptr;
        framebufferCi.flags = 0;
        framebufferCi.renderPass = rPass;
        framebufferCi.attachmentCount = 2;
        framebufferCi.width = rDevice.windowSize().width;
        framebufferCi.height = rDevice.windowSize().height;
        framebufferCi.layers = 1;

        framebuffers.resize(rDevice.numDisplayBuffers());
        for (uint32_t i = 0; i < rDevice.numDisplayBuffers(); ++i) {
            VkImageView attachments[2] = { rDevice.swpachainImageView(i), rDevice.depthBuffer() };
            framebufferCi.pAttachments = attachments;
            result = vkCreateFramebuffer(rDevice.device(), &framebufferCi, nullptr, &framebuffers[i]);
        }

        VkCommandBufferAllocateInfo commandBufferCi; 
        commandBufferCi.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        commandBufferCi.pNext = nullptr;
        commandBufferCi.commandPool = rDevice.commandPool();
        commandBufferCi.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        commandBufferCi.commandBufferCount = 1;

        result = vkAllocateCommandBuffers(rDevice.device(), &commandBufferCi, &commandBuffer);

        VkSemaphoreCreateInfo semaphoreCi;
        semaphoreCi.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        semaphoreCi.pNext = nullptr;
        semaphoreCi.flags = 0;

        result = vkCreateSemaphore(rDevice.device(), &semaphoreCi, nullptr, &swapchainImageSemaphore);

        //mid render elements partially completed during recording buffer

        bufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        bufferBeginInfo.pNext = nullptr;
        bufferBeginInfo.flags = 0;
        bufferBeginInfo.pInheritanceInfo = nullptr;

        clear_values[0].color.float32[0] = 0.0f;
        clear_values[0].color.float32[1] = 0.0f;
        clear_values[0].color.float32[2] = 0.0f;
        clear_values[0].color.float32[3] = 0.0f;
        clear_values[1].depthStencil.depth = 1.0f;
        clear_values[1].depthStencil.stencil = 0;

        renderPassBegin.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassBegin.pNext = nullptr;
        renderPassBegin.renderPass = rPass;
        renderPassBegin.renderArea.offset = { 0, 0 };
        renderPassBegin.renderArea.extent = rDevice.windowSize(); //Dynamic??
        renderPassBegin.clearValueCount = 2;
        renderPassBegin.pClearValues = clear_values;

        VkFenceCreateInfo fenceCi;
        fenceCi.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceCi.pNext = nullptr;
        fenceCi.flags = 0;
        result = vkCreateFence(rDevice.device(), &fenceCi, nullptr, &fence);

        pipelineFlags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.pNext = nullptr;
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = &swapchainImageSemaphore;
        submitInfo.pWaitDstStageMask = &pipelineFlags;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;
        submitInfo.signalSemaphoreCount = 0;
        submitInfo.pSignalSemaphores = nullptr;
    }

    Scene::~Scene()
    {
        vkDestroyFence(rDevice.device(), fence, nullptr);
        vkDestroySemaphore(rDevice.device(), swapchainImageSemaphore, nullptr);

        for (auto fb : framebuffers) {
            vkDestroyFramebuffer(rDevice.device(), fb, nullptr);
        }

        vkDestroyRenderPass(rDevice.device(), rPass, nullptr);
    }

    void Scene::addToScene(SceneElement * element)
    {
        objectsList.push_back(element);
    }

    void Scene::render()
    {
        VkResult result;

        result = vkResetFences(rDevice.device(), 1, &fence);
        result = vkBeginCommandBuffer(commandBuffer, &bufferBeginInfo);

        uint32_t currentImageIndex;
        result = vkAcquireNextImageKHR(rDevice.device(), rDevice.swapchain(), UINT64_MAX, swapchainImageSemaphore, VK_NULL_HANDLE, &currentImageIndex);

        renderPassBegin.framebuffer = framebuffers[currentImageIndex];
        vkCmdBeginRenderPass(commandBuffer, &renderPassBegin, VK_SUBPASS_CONTENTS_INLINE);

        for (auto element : objectsList) {
            element->recordToCmdBuffer(commandBuffer);
        }

        vkCmdEndRenderPass(commandBuffer);
        vkEndCommandBuffer(commandBuffer);

        result = vkQueueSubmit(rDevice.graphicsQueue(), 1, &submitInfo, fence);

        VkPresentInfoKHR present;
        present.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        present.pNext = nullptr;
        present.swapchainCount = 1;
        present.pSwapchains = &(rDevice.swapchain());
        present.pImageIndices = &currentImageIndex;
        present.waitSemaphoreCount = 0;
        present.pWaitSemaphores = nullptr;
        present.pResults = nullptr;

        VkResult waitResult;
        do {
            waitResult = vkWaitForFences(rDevice.device(), 1, &fence, VK_TRUE, UINT64_MAX);
        } while (waitResult == VK_TIMEOUT);

        vkQueuePresentKHR(rDevice.presentQueue(), &present);
    }

    base::Device & Scene::rendererDevice() noexcept
    {
        return rDevice;
    }

    VkRenderPass Scene::renderPass() noexcept
    {
        return rPass;
    }
}
