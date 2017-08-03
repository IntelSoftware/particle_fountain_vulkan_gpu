#ifndef _PSIM_BUFFER_
#define _PSIM_BUFFER_

#include <memory>
#include <psim/particle.h>

/*
NOTE: Sort the buffer before adding any new elements
NOTE: For performance reasons access to the buffer is not boundary check to restrict operation only to active elements. This has to be maintained manually.
NOTE: The preferred order of operations is: 1)Compute on active particles, 2) sort, 3) Optionally add id space available
*/

namespace psim {
    class Buffer final
    {
    public:
        explicit Buffer(std::size_t size);
        ~Buffer();
        Buffer(const Buffer &) = delete;
        Buffer& operator=(const Buffer &) = delete;

        std::size_t count() const noexcept;
        std::size_t activeCount() const noexcept;
        std::size_t inactiveIndex() const noexcept;

        void sort();
        bool add(const particle& p);
        particle& operator[](std::size_t idx);

        void* data() noexcept;
        std::size_t dataSizeTotal() const noexcept;
        std::size_t dataSizeActive() const noexcept;

    private:
        std::size_t totalSize;
        std::size_t firstInactiveIdx;
        particle* bufferData;
    };
}

#include <psim/buffer_inline.h>

#endif
