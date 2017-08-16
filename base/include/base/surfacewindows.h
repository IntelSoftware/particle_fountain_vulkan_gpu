#ifndef _RENDERER_WINDOWS_SURFACE_
#define _RENDERER_WINDOWS_SURFACE_

#include <base/vk_windows.h>
#include <base/appinstance.h>
#include <stdexcept>

namespace base {
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
