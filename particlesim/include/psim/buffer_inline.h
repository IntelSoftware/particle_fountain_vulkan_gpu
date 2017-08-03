#ifndef _PSIM_BUFFER_INLINE_
#define _PSIM_BUFFER_INLINE_

namespace psim {
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

    inline particle & Buffer::operator[](std::size_t idx)
    {
        return bufferData[idx];
    }

    inline void* Buffer::data() noexcept
    {
        return bufferData;
    }

    inline std::size_t Buffer::dataSizeTotal() const noexcept
    {
        return particle::size() * count();
    }

    inline std::size_t Buffer::dataSizeActive() const noexcept
    {
        return particle::size() * activeCount();
    }
}
#endif // !_PSIM_BUFFER_INLINE_
