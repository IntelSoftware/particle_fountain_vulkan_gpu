#ifndef _BASE_BUFFER_INLINE_
#define _BASE_BUFFER_INLINE_

namespace base {
    inline std::size_t Buffer::count() const noexcept
    {
        return totalSize;
    }

    inline std::size_t Buffer::activeCount() const noexcept
    {
        return firstInactiveIdx;
    }

    inline std::size_t Buffer::inactiveIndex() const noexcept
    {
        return firstInactiveIdx;
    }

    inline particle* Buffer::data() noexcept
    {
        return mmapData;
    }

    inline VkBuffer Buffer::dataBuffer() noexcept
    {
        return buffer;
    }

    inline std::size_t Buffer::dataSizeTotal() const noexcept
    {
        return particle::size() * count();
    }

    inline std::size_t Buffer::dataSizeActive() const noexcept
    {
        return particle::size() * activeCount();
    }

    inline VkMemoryRequirements Buffer::memRequirements() const noexcept
    {
        return bufferMemoryRequirements;
    }
}
#endif // !_PSIM_BUFFER_INLINE_
