#ifndef _BASE_BUFFER_
#define _BASE_BUFFER_

#include <base/particle.h>
#include <base/device.h>

/*
NOTE: Sort the buffer before adding any new elements
NOTE: For performance reasons access to the buffer is not boundary check to restrict operation only to active elements. This has to be maintained manually.
NOTE: The preferred order of operations is: 1)Compute on active particles, 2) sort, 3) Optionally add id space available
*/

namespace base {
    class Buffer final
    {
    public:
        explicit Buffer(std::size_t size, Device& dev);
        ~Buffer();
        Buffer(const Buffer &) = delete;
        Buffer& operator=(const Buffer &) = delete;

        std::size_t count() const noexcept;
        std::size_t activeCount() const noexcept;
        std::size_t inactiveIndex() const noexcept;

        void sort();
        bool add(const particle& p);
		//GPU_TP43
        particle* Buffer::data() noexcept;
        VkBuffer dataBuffer() noexcept;
        VkResult mapToHost();
        void unmapFromHost();
        VkMemoryRequirements memRequirements() const noexcept;

        std::size_t dataSizeTotal() const noexcept;
        std::size_t dataSizeActive() const noexcept;


    private:
        std::size_t totalSize;
        std::size_t firstInactiveIdx;
        Device& device;

        particle* mmapData;

        VkMemoryRequirements bufferMemoryRequirements;

        VkBuffer buffer;
        VkDeviceMemory bufferMemory;

    };
}

#include <base/buffer_inline.h>

#endif
