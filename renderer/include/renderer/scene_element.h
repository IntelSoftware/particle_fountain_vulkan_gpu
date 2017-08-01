#ifndef _RENDERER_SCENE_ELEMENT
#define _RENDERER_SCENE_ELEMENT

#include <renderer/device.h>

namespace renderer {
	class SceneElement
	{
	public:
		SceneElement(Device &device);
		virtual ~SceneElement() = default;

		virtual void recordToCmdBuffer(VkCommandBuffer commandBuffer) = 0;
		
	protected:
		Device& renderDevice;
	};
}

#endif