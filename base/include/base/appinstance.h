#ifndef _BASE_APP_INSTANCE_
#define _BASE_APP_INSTANCE_

#include <base/vk_windows.h>
#include <vector>

namespace base {
    class AppInstance
    {
    public:
        AppInstance();
        ~AppInstance();
        AppInstance(const AppInstance&) = delete;
        AppInstance& operator=(const AppInstance&) = delete;

        VkInstance& instance() noexcept;

        std::vector<VkPhysicalDevice> physicalDevices() const noexcept;
        VkPhysicalDeviceProperties deviceProperties(VkPhysicalDevice device) const noexcept;
        VkPhysicalDeviceMemoryProperties deviceMemoryProperties(VkPhysicalDevice device) const noexcept;
        std::vector<VkQueueFamilyProperties> deviceQueueProperties(VkPhysicalDevice device) const noexcept;

        void printHardware() const noexcept;
        void printPhysicalDevice(VkPhysicalDevice device) const noexcept;
    protected:
        VkInstance inst;

    };
}

#endif
