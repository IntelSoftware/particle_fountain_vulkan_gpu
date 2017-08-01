#ifndef _RENDERER_WINDOWS_SURFACE_
#define _RENDERER_WINDOWS_SURFACE_

#include <renderer/vk_windows.h>
#include <renderer/appinstance.h>
#include <stdexcept>

namespace renderer {
	class SurfaceWindows final
	{
	public:
		SurfaceWindows(AppInstance& inst, HINSTANCE hinstance, HWND hwnd);
		~SurfaceWindows();

		VkSurfaceKHR& surface();
	private:
		AppInstance& appInstance;
		VkSurfaceKHR wndSurface;
	};
}

#endif
