#ifndef _RENDERER_SCENE_ELEMENT
#define _RENDERER_SCENE_ELEMENT

#include <base/device.h>

namespace renderer {
    class SceneElement
    {
    public:
        SceneElement(base::Device &device);
        virtual ~SceneElement() = default;

        virtual void recordToCmdBuffer(VkCommandBuffer commandBuffer) = 0;
		
    protected:
        base::Device& renderDevice;
    };
}

#endif
