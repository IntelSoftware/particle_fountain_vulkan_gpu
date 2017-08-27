#include <base/buffer.h>

namespace base {
    Buffer::Buffer(std::size_t size, Device& dev)
        : totalSize(size)
        , firstInactiveIdx(0)
        , device(dev)
        , mmapData(nullptr)
    {
        VkResult result;

		//GPU_TP42
        VkBufferCreateInfo buffCi;
        buffCi.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        buffCi.pNext = nullptr;
        buffCi.flags = 0;
        buffCi.size = particle::size() * totalSize;
        buffCi.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
        buffCi.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        buffCi.queueFamilyIndexCount = 0;
        buffCi.pQueueFamilyIndices = nullptr;

        result = vkCreateBuffer(device.device(), &buffCi, nullptr, &buffer);
        if(result != VK_SUCCESS) {
            throw std::runtime_error("Could not create VkBuffer in Buffer");
        }

        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(device.physicalDevice(), &memProperties);

        vkGetBufferMemoryRequirements(device.device(), buffer, &bufferMemoryRequirements);


        bool memoryTypeFound = false;
        uint32_t memTypeIndex;
        auto memTags = (VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        for (memTypeIndex = 0; memTypeIndex < memProperties.memoryTypeCount; ++memTypeIndex) {
            if ((bufferMemoryRequirements.memoryTypeBits & (1<< memTypeIndex)) &&
               ((memProperties.memoryTypes[memTypeIndex].propertyFlags & memTags) == memTags)) {
                memoryTypeFound = true;
                break;
            }
        }

        if (!memoryTypeFound) {
            throw std::runtime_error("Could not find proper memory type for vertex buffer");
        }

        VkMemoryAllocateInfo buffAlloc;
        buffAlloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        buffAlloc.pNext = nullptr;
        buffAlloc.memoryTypeIndex = memTypeIndex;
        buffAlloc.allocationSize = bufferMemoryRequirements.size;

        result = vkAllocateMemory(device.device(), &buffAlloc, nullptr, &bufferMemory);
        result = vkBindBufferMemory(device.device(), buffer, bufferMemory, 0);

        mapToHost();
        for(std::size_t i = 0; i< totalSize; ++i) {
            mmapData[i].ttl = -1;
        }
        unmapFromHost();

    }

    Buffer::~Buffer()
    {
        if(mmapData != nullptr) {
            unmapFromHost();
        }

        vkDestroyBuffer(device.device(), buffer, nullptr);
        vkFreeMemory(device.device(), bufferMemory, nullptr);
    }

    void Buffer::sort()
    {
        if(mmapData == nullptr) {
            throw std::runtime_error("Buffer memory not mapped during sort opertation");
        }

        if (totalSize >= 2) {
            std::size_t l = 0;
            std::size_t r = totalSize-1;

            bool done = false;
            while (l < r) {
                while ( (l < r) && (mmapData[l].ttl > 0) ) {
                    ++l;
                }

                while ( (r > l) && (mmapData[r].ttl <= 0) ) {
                    --r;
                }

                if (l != r) {
                    particle tmp = mmapData[r];
                    mmapData[r] = mmapData[l];
                    mmapData[l] = tmp;
                    ++l;
                    firstInactiveIdx = r;
                    --r;
                } 
            }
        }
    }

    bool Buffer::add(const particle & p)
    {
        if(mmapData == nullptr) {
            throw std::runtime_error("Buffer memory not mapped during add opertation");
        }

        bool ret;
        if (firstInactiveIdx >= totalSize) {
            ret = false;
        } else {
            mmapData[firstInactiveIdx] = p;
            ++firstInactiveIdx;
            ret = true;
        }

        return ret;
    }
    
    VkResult Buffer::mapToHost()
    {
        return vkMapMemory(device.device(), 
                bufferMemory, 
                0, 
                bufferMemoryRequirements.size, 
                0, 
                &reinterpret_cast<void*>(mmapData)
                );
    }

    void Buffer::unmapFromHost()
    {
        vkUnmapMemory(device.device(), bufferMemory);
        mmapData = nullptr;
    }
}
