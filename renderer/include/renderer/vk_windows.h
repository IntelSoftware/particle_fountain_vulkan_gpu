#ifndef _RENDERER_WIN_SUPPORT_
#define _RENDERER_WIN_SUPPORT_

#ifdef _WIN32
	#define VK_USE_PLATFORM_WIN32_KHR
	#include <vulkan/vulkan.h>
#else
	#error Only Windows plaftorm supported at this time
#endif

#endif // !_RENDERER_WIN_SUPPORT_
