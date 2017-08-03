#ifndef _RENDERER_DEVICE_
#define _RENDERER_DEVICE_

#include <renderer/appinstance.h>
#include <forward_list>

namespace renderer {
    class Device
    {
    public:
        struct DeviceTypeReq {
            bool deviceTypeReq;
            VkPhysicalDeviceType deviceType;
            bool vendorIDReq;
            uint32_t vendorID;
            bool  deviceIDReq;
            uint32_t deviceID;
        };

        Device(AppInstance& inst, VkSurfaceKHR surface); //select any first able to initialize
        Device(AppInstance& inst, VkSurfaceKHR surface, const DeviceTypeReq& typeReq); //select any first able to initialize
        Device(AppInstance& inst, VkSurfaceKHR surface, VkPhysicalDevice device); // select gien
        ~Device();

        VkDevice& device() noexcept;
        AppInstance& instance() noexcept;
        VkPhysicalDevice physicalDevice() noexcept;

        VkSurfaceFormatKHR swapchainSurfaceFormat() const noexcept;
        VkFormat depthBufferFormat() const noexcept;
        VkExtent2D windowSize() const noexcept;
        uint32_t numDisplayBuffers() const noexcept;
        VkImageView swpachainImageView(uint32_t idx);
        VkImageView depthBuffer() const noexcept;
        VkSwapchainKHR& swapchain() noexcept;
        
        VkCommandPool commandPool() const noexcept;
        VkQueue graphicsQueue() const noexcept;
        VkQueue presentQueue() const noexcept;


    protected:
        AppInstance& appInstance;
        VkSurfaceKHR wndSurface;
        VkPhysicalDevice physDevice;

        VkDevice virtualDevice;
        VkQueue deviceGraphicsQueue;
        VkSurfaceFormatKHR surfaceFormatSelected;
        VkSwapchainKHR deviceSwapchain;
        std::vector<VkImage> swapchainImages;
        std::vector<VkImageView> swapchainImageViews;
        VkCommandPool graphicsCommandPool;

        VkImage depthBufferImage;
        VkDeviceMemory depthBufferDeviceMemory;
        VkImageView depthBufferImageView;

        VkExtent2D wndSize;

        bool checkAgainstTypeReq(VkPhysicalDevice device, const DeviceTypeReq& typeReq) const noexcept;
        VkResult createDevice(VkPhysicalDevice device, VkSurfaceKHR surface);
    };
}

#endif
