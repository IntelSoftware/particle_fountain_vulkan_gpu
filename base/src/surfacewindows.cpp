#include <base/surfacewindows.h>

namespace base {
    SurfaceWindows::SurfaceWindows(AppInstance & inst, HINSTANCE hinstance, HWND hwnd)
        : appInstance(inst)
    {
        VkWin32SurfaceCreateInfoKHR ci;
        ci.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
        ci.pNext = nullptr;
        ci.hinstance = hinstance;
        ci.hwnd = hwnd;
        ci.flags = 0;

        if (vkCreateWin32SurfaceKHR(appInstance.instance(), &ci, nullptr, &wndSurface) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create windows surface");
        }
    }

    SurfaceWindows::~SurfaceWindows()
    {
        vkDestroySurfaceKHR(appInstance.instance(), wndSurface, nullptr);
    }

    VkSurfaceKHR& SurfaceWindows::surface()
    {
        return wndSurface;
    }
}
