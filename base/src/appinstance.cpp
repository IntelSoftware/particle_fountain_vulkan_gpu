#include <stdexcept>
#include <iostream>
#include <string>
#include <iostream>

#include <base/appinstance.h>

namespace base {
    AppInstance::AppInstance()
    {
        VkApplicationInfo appInfo;
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pNext = nullptr;
        appInfo.pApplicationName = "Particle System Example";
        appInfo.applicationVersion = VK_MAKE_VERSION(0, 1, 0);
        appInfo.pEngineName = "Vulkan Simple Renderer";
        appInfo.engineVersion = VK_MAKE_VERSION(0, 1, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;

        const uint32_t instExtCount = 2;
        const char* const instExtList[] = { VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_WIN32_SURFACE_EXTENSION_NAME };

        VkInstanceCreateInfo instInfo;
        instInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instInfo.pNext = nullptr;
        instInfo.flags = 0;
        instInfo.pApplicationInfo = &appInfo;
        instInfo.enabledExtensionCount = instExtCount;
        instInfo.ppEnabledExtensionNames = instExtList;
#ifdef DEBUG
        std::clog << "AppInstance: VULKAN DEBUG LAYERS ENABLED\n";

        const char* const layerList[] = { "VK_LAYER_LUNARG_standard_validation" };

        instInfo.enabledLayerCount = 1;
        instInfo.ppEnabledLayerNames = layerList;
#else
        instInfo.enabledLayerCount = 0;
        instInfo.ppEnabledLayerNames = nullptr;
#endif // DEBUG

        VkResult res = vkCreateInstance(&instInfo, nullptr, &inst);
                
        if (res != VK_SUCCESS) {
            throw std::runtime_error("Could not create Vulkan instance");
        }
    }

    AppInstance::~AppInstance()
    {
        vkDestroyInstance(inst, nullptr);
    }

    VkInstance & AppInstance::instance() noexcept
    {
        return inst;
    }

    std::vector<VkPhysicalDevice> AppInstance::physicalDevices() const noexcept
    {
        uint32_t count;
        std::vector<VkPhysicalDevice> ret;

        vkEnumeratePhysicalDevices(inst, &count, nullptr);

        if (count > 0) {
            ret.resize(count);
            vkEnumeratePhysicalDevices(inst, &count, ret.data());
        }

        return ret;
    }

    VkPhysicalDeviceProperties AppInstance::deviceProperties(VkPhysicalDevice device) const noexcept
    {
        VkPhysicalDeviceProperties ret;

        vkGetPhysicalDeviceProperties(device, &ret);

        return ret;
    }
        
    VkPhysicalDeviceMemoryProperties AppInstance::deviceMemoryProperties(VkPhysicalDevice device) const noexcept
    {
        VkPhysicalDeviceMemoryProperties ret;

        vkGetPhysicalDeviceMemoryProperties(device, &ret);

        return ret;
    }

    std::vector<VkQueueFamilyProperties> AppInstance::deviceQueueProperties(VkPhysicalDevice device) const noexcept
    {
        uint32_t count;
        std::vector<VkQueueFamilyProperties> ret;

        vkGetPhysicalDeviceQueueFamilyProperties(device, &count, nullptr);

        if (count > 0) {
            ret.resize(count);
            vkGetPhysicalDeviceQueueFamilyProperties(device, &count, ret.data());
        }

        return ret;
    }

    void AppInstance::printHardware() const noexcept
    {
        auto devList = physicalDevices();
        for (auto dev : devList) {
            printPhysicalDevice(dev);
        }
    }

    void AppInstance::printPhysicalDevice(VkPhysicalDevice device) const noexcept
    {
        auto propList = deviceProperties(device);

        std::string devTypeText;
        switch (propList.deviceType) {
        case VK_PHYSICAL_DEVICE_TYPE_OTHER: devTypeText = " (OTHER) "; break;
        case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU: devTypeText = " (INTEGRATED_GPU) "; break;
        case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU: devTypeText = " (DISCRETE_GPU) "; break;
        case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU: devTypeText = " (VIRTUAL_GPU) "; break;
        case VK_PHYSICAL_DEVICE_TYPE_CPU: devTypeText = " (CPU) "; break;
        }

        std::clog << "DEVICE PROPERTIES:"
            << "\n deviceName: " << propList.deviceName
            << "\n deviceType: " << propList.deviceType << devTypeText
            << "\n apiVersion: " << propList.apiVersion
            << "\n driverVersion: " << propList.driverVersion
            << "\n vendorID: " << propList.vendorID
            << "\n deviceID: " << propList.deviceID
            << std::endl;

        auto queueList = deviceQueueProperties(device);
        for (auto propQueue : queueList) {
            std::string queueFlagsText;
            queueFlagsText += (propQueue.queueFlags & VK_QUEUE_GRAPHICS_BIT) ? " GRAPHICS_BIT" : "";
            queueFlagsText += (propQueue.queueFlags & VK_QUEUE_COMPUTE_BIT) ? " COMPUTE_BIT" : "";
            queueFlagsText += (propQueue.queueFlags & VK_QUEUE_TRANSFER_BIT) ? " TRANSFER_BIT" : "";
            queueFlagsText += (propQueue.queueFlags & VK_QUEUE_SPARSE_BINDING_BIT) ? " SPARSE_BINDING_BIT" : "";

            std::clog << "\n DEVICE QUEUE:"
                << "\n  queueFlags: " << queueFlagsText
                << "\n  queueCount: " << propQueue.queueCount
                << std::endl;
        }
    }
}
