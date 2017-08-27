#include <base/device.h>
#include <iostream>

namespace base {
    Device::Device(AppInstance & inst, VkSurfaceKHR surface)
        : appInstance(inst)
        , wndSurface(surface)
    {
        auto devList = appInstance.physicalDevices();
        for (auto dev : devList) {
            if (createDevice(dev, wndSurface) == VK_SUCCESS) {
                return;
            }
        }

        throw std::runtime_error("Could not find suitable physical device");
    }

	Device::Device(AppInstance & inst, VkSurfaceKHR surface, uint32_t vendorID)
		: appInstance(inst)
		, wndSurface(surface)
	{
		auto devList = appInstance.physicalDevices();
		for (auto dev : devList) {
			auto devProps = appInstance.deviceProperties(dev);
			if ((devProps.vendorID == vendorID) && (createDevice(dev, wndSurface) == VK_SUCCESS)) {
				return;
			}
		}
		for (auto dev : devList) {
			auto devProps = appInstance.deviceProperties(dev);
			if ((devProps.vendorID != vendorID) && (createDevice(dev, wndSurface) == VK_SUCCESS)) {
				return;
			}
		}

		throw std::runtime_error("Could not find suitable physical device");
	}

    Device::Device(AppInstance & inst, VkSurfaceKHR surface, const DeviceTypeReq & typeReq)
        : appInstance(inst)
        , wndSurface(surface)
    {
        auto devList = appInstance.physicalDevices();
        for (auto dev : devList) {
            if (checkAgainstTypeReq(dev, typeReq) == true) {
                if (createDevice(dev, surface) == VK_SUCCESS) {
                    return;
                }
            }
        }

        throw std::runtime_error("Could not find suitable physical device");
    }

    Device::Device(AppInstance & inst, VkSurfaceKHR surface, VkPhysicalDevice device)
        : appInstance(inst)
        , wndSurface(surface)
    {
        if (createDevice(device, surface) != VK_SUCCESS) {
            throw std::runtime_error("Could not instantiate virtual device from given physical device");
        }
    }

    Device::~Device()
    {
        vkDestroyImageView(virtualDevice, depthBufferImageView, nullptr);
        vkDestroyImage(virtualDevice, depthBufferImage, nullptr);
        vkFreeMemory(virtualDevice, depthBufferDeviceMemory, nullptr);

        for (auto imgView : swapchainImageViews) {
            vkDestroyImageView(virtualDevice, imgView, nullptr);
        }

        vkDestroySwapchainKHR(virtualDevice, deviceSwapchain, nullptr);
        vkDestroyCommandPool(virtualDevice, graphicsCommandPool, nullptr);
        vkDestroyDevice(virtualDevice, nullptr);
    }

    VkDevice & Device::device() noexcept
    {
        return virtualDevice;
    }

    AppInstance & Device::instance() noexcept
    {
        return appInstance;
    }

    VkPhysicalDevice Device::physicalDevice() noexcept
    {
        return physDevice;
    }

    VkSurfaceFormatKHR Device::swapchainSurfaceFormat() const noexcept
    {
        return surfaceFormatSelected;
    }

    VkFormat Device::depthBufferFormat() const noexcept
    {
        return VK_FORMAT_D16_UNORM;
    }

    VkExtent2D Device::windowSize() const noexcept
    {
        return wndSize;
    }

    uint32_t Device::numDisplayBuffers() const noexcept
    {
        return 2;
    }

    VkImageView Device::swpachainImageView(uint32_t idx)
    {
        return swapchainImageViews[idx];
    }

    VkImageView Device::depthBuffer() const noexcept
    {
        return depthBufferImageView;
    }

    VkSwapchainKHR& Device::swapchain() noexcept
    {
        return deviceSwapchain;
    }

    VkCommandPool Device::commandPool() const noexcept
    {
        return graphicsCommandPool;
    }

    VkQueue Device::graphicsQueue() const noexcept
    {
        return deviceGraphicsQueue;
    }

    VkQueue Device::presentQueue() const noexcept
    {
        return deviceGraphicsQueue;
    }

    VkQueue Device::computeQueue() const noexcept
    {
        return deviceGraphicsQueue;
    }

    bool Device::checkAgainstTypeReq(VkPhysicalDevice device, const DeviceTypeReq & typeReq) const noexcept
    {
        auto devProps = appInstance.deviceProperties(device);

        if ((typeReq.deviceIDReq && (devProps.deviceID != typeReq.deviceID))
            || (typeReq.deviceTypeReq && (devProps.deviceType != typeReq.deviceType))
            || (typeReq.vendorIDReq && (devProps.vendorID != typeReq.vendorID))) {
            return false;
        } else {
            return true;
        }
    }

    VkResult Device::createDevice(VkPhysicalDevice device, VkSurfaceKHR surface)
    {
        //TODO: V0.1 improve later
        physDevice = device;
        VkResult result;

        auto queueProps = appInstance.deviceQueueProperties(device);
        uint32_t queueIdx;
        bool queueFound = false;
        for (uint32_t i = 0; i < queueProps.size(); ++i) {
            if ((queueProps[i].queueFlags & (VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT)) 
                    == (VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT)) {
                VkBool32 support;
                vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &support);
                if (support == VK_TRUE) {
                    queueIdx = i;
                    queueFound = true;
                    break;
                }
            }
        }

        if (!queueFound) {
            return VK_ERROR_INITIALIZATION_FAILED;
        }

        const float priorities[] = { 0.0f };
        VkDeviceQueueCreateInfo queueCi;
        queueCi.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCi.pNext = nullptr;
        queueCi.flags = 0;
        queueCi.queueFamilyIndex = queueIdx;
        queueCi.queueCount = 1;
        queueCi.pQueuePriorities = priorities;

        const char* devExtensions[] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

        VkDeviceCreateInfo devCi;
        devCi.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        devCi.pNext = nullptr;
        devCi.flags = 0;
        devCi.queueCreateInfoCount = 1;
        devCi.pQueueCreateInfos = &queueCi;
        devCi.enabledLayerCount = 0;
        devCi.ppEnabledLayerNames = nullptr;
        devCi.enabledExtensionCount = 1;
        devCi.ppEnabledExtensionNames = devExtensions;
        devCi.pEnabledFeatures = nullptr;

        result = vkCreateDevice(device, &devCi, nullptr, &virtualDevice);
        if (result != VK_SUCCESS) {
            return result;
        }

        vkGetDeviceQueue(virtualDevice, queueIdx, 0, &deviceGraphicsQueue);

        VkCommandPoolCreateInfo cmdPoolCi;
        cmdPoolCi.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        cmdPoolCi.pNext = nullptr;
        cmdPoolCi.flags = (VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
        cmdPoolCi.queueFamilyIndex = queueIdx;

        vkCreateCommandPool(virtualDevice, &cmdPoolCi, nullptr, &graphicsCommandPool);

        uint32_t surfaceFormatCount;
        std::vector<VkSurfaceFormatKHR> surfaceFormatList;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &surfaceFormatCount, nullptr);
        surfaceFormatList.resize(surfaceFormatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &surfaceFormatCount, surfaceFormatList.data());
        if (surfaceFormatList[0].format == VK_FORMAT_UNDEFINED) {
            surfaceFormatSelected = { VK_FORMAT_R8G8B8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
        } else {
            surfaceFormatSelected = surfaceFormatList[0];
        }

        VkSurfaceCapabilitiesKHR surfaceCapabilities;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &surfaceCapabilities);

        if (surfaceCapabilities.minImageCount > numDisplayBuffers()) {
            vkDestroyDevice(virtualDevice, nullptr);
            return VK_ERROR_INITIALIZATION_FAILED;
        }

        if (surfaceCapabilities.currentExtent.height == 0xFFFFFFFF) {
            wndSize = { 1024, 768 };
        } else {
            wndSize = surfaceCapabilities.currentExtent;
        }

        VkCompositeAlphaFlagBitsKHR compositeAlphaFlag;
        if (surfaceCapabilities.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR) {
            compositeAlphaFlag = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        } else if (surfaceCapabilities.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR) {
            compositeAlphaFlag = VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR;
        } else if (surfaceCapabilities.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR) {
            compositeAlphaFlag = VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR;
        } else {
            compositeAlphaFlag = VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR;
        }

        uint32_t surfacePresentModesCount;
        std::vector<VkPresentModeKHR> surfacePresentModesList;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &surfacePresentModesCount, nullptr);
        surfacePresentModesList.resize(surfacePresentModesCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &surfacePresentModesCount, surfacePresentModesList.data());
        //TODO: experiment with present modes and image count
        
        VkSwapchainCreateInfoKHR swapchainCi;
        swapchainCi.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapchainCi.pNext = nullptr;
        swapchainCi.flags = 0;
        swapchainCi.surface = surface;
        swapchainCi.minImageCount = numDisplayBuffers();
        swapchainCi.imageFormat = surfaceFormatSelected.format;
        swapchainCi.imageColorSpace = surfaceFormatSelected.colorSpace;
        swapchainCi.imageExtent = wndSize;
        swapchainCi.imageArrayLayers = 1;
        swapchainCi.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        swapchainCi.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swapchainCi.queueFamilyIndexCount = 0;
        swapchainCi.pQueueFamilyIndices = nullptr;
        swapchainCi.preTransform = surfaceCapabilities.currentTransform;
        swapchainCi.compositeAlpha = compositeAlphaFlag;
        swapchainCi.presentMode = surfacePresentModesList[0];
        swapchainCi.clipped = VK_TRUE;
        swapchainCi.oldSwapchain = nullptr;

        result = vkCreateSwapchainKHR(virtualDevice, &swapchainCi, nullptr, &deviceSwapchain);
        if (result != VK_SUCCESS) {
            return result;
        }

        uint32_t swapchainImagesCount;

        vkGetSwapchainImagesKHR(virtualDevice, deviceSwapchain, &swapchainImagesCount, nullptr);
        swapchainImages.resize(swapchainImagesCount);
        swapchainImageViews.resize(swapchainImagesCount);
        vkGetSwapchainImagesKHR(virtualDevice, deviceSwapchain, &swapchainImagesCount, swapchainImages.data());

        for (uint32_t i = 0; i < swapchainImagesCount; ++i) {
            VkImageViewCreateInfo imgViewCi;
            imgViewCi.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            imgViewCi.pNext = nullptr;
            imgViewCi.flags = 0;
            imgViewCi.image = swapchainImages[i];
            imgViewCi.viewType = VK_IMAGE_VIEW_TYPE_2D;
            imgViewCi.format = surfaceFormatSelected.format;
            imgViewCi.components = VkComponentMapping{ 
                                        VK_COMPONENT_SWIZZLE_R,
                                        VK_COMPONENT_SWIZZLE_G,
                                        VK_COMPONENT_SWIZZLE_B,
                                        VK_COMPONENT_SWIZZLE_A };
            imgViewCi.subresourceRange = VkImageSubresourceRange{ 
                                        VK_IMAGE_ASPECT_COLOR_BIT,
                                        0,
                                        1,
                                        0,
                                        1 };
             vkCreateImageView(virtualDevice, &imgViewCi, nullptr, &swapchainImageViews[i]);
        }

        //TODO: Add more robbyst format picking, vkPhysicalDeviceFormatProperties 

        VkImageCreateInfo depthBufferCi;
        depthBufferCi.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        depthBufferCi.pNext = nullptr;
        depthBufferCi.flags = 0;
        depthBufferCi.imageType = VK_IMAGE_TYPE_2D;
        depthBufferCi.format = VK_FORMAT_D16_UNORM;
        depthBufferCi.extent = VkExtent3D{ wndSize.width, wndSize.height, 1 };
        depthBufferCi.mipLevels = 1;
        depthBufferCi.arrayLayers = 1;
        depthBufferCi.samples = VK_SAMPLE_COUNT_1_BIT;
        depthBufferCi.tiling = VK_IMAGE_TILING_OPTIMAL;
        depthBufferCi.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        depthBufferCi.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        depthBufferCi.queueFamilyIndexCount = 0;
        depthBufferCi.pQueueFamilyIndices = nullptr;
        depthBufferCi.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

        vkCreateImage(virtualDevice, &depthBufferCi, nullptr, &depthBufferImage);

        VkMemoryRequirements depthBufferMemReq;
        vkGetImageMemoryRequirements(virtualDevice, depthBufferImage, &depthBufferMemReq);

        VkPhysicalDeviceMemoryProperties memProperties = appInstance.deviceMemoryProperties(device);

        uint32_t memTypeIndex;
        bool memTypeFound = false;
        for (memTypeIndex = 0; memTypeIndex < memProperties.memoryTypeCount; ++memTypeIndex) {
            if((depthBufferMemReq.memoryTypeBits & (1 << memTypeIndex)) &&
               (memProperties.memoryTypes[memTypeIndex].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)) {
                    memTypeFound = true;
                    break;
            }
        }

        if (!memTypeFound) {
            return VK_ERROR_INITIALIZATION_FAILED;
        }

        VkMemoryAllocateInfo depthBufferMemAlloc;
        depthBufferMemAlloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        depthBufferMemAlloc.pNext = nullptr;
        depthBufferMemAlloc.allocationSize = depthBufferMemReq.size;
        depthBufferMemAlloc.memoryTypeIndex = memTypeIndex;

        vkAllocateMemory(virtualDevice, &depthBufferMemAlloc, nullptr, &depthBufferDeviceMemory);
        vkBindImageMemory(virtualDevice, depthBufferImage, depthBufferDeviceMemory, 0);

        VkImageViewCreateInfo depthBufferImageViewCi;
        depthBufferImageViewCi.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        depthBufferImageViewCi.pNext = nullptr;
        depthBufferImageViewCi.flags = 0;
        depthBufferImageViewCi.image = depthBufferImage;
        depthBufferImageViewCi.viewType = VK_IMAGE_VIEW_TYPE_2D;
        depthBufferImageViewCi.format = VK_FORMAT_D16_UNORM;
        depthBufferImageViewCi.components = VkComponentMapping{ 
                                                VK_COMPONENT_SWIZZLE_R,
                                                VK_COMPONENT_SWIZZLE_G,
                                                VK_COMPONENT_SWIZZLE_B,
                                                VK_COMPONENT_SWIZZLE_A };
        depthBufferImageViewCi.subresourceRange = VkImageSubresourceRange{ 
                                                VK_IMAGE_ASPECT_DEPTH_BIT,
                                                0,
                                                1,
                                                0,
                                                1 };

        vkCreateImageView(virtualDevice, &depthBufferImageViewCi, nullptr, &depthBufferImageView);

		std::clog << "SELECTED PHYSICAL DEVICE:\n";
		appInstance.printPhysicalDevice(device);

        return VK_SUCCESS;
    }
}
