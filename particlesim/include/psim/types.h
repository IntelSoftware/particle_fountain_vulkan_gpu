#ifndef _PSIM_TYPES_
#define _PSIM_TYPES_

#include <cstddef>
#include <utility>
#include <cmath>

namespace psim {
    enum rgbIndex {
        RED = 0x0,
        GREEN = 0x1,
        BLUE = 0x2
    };

    enum axis {
        X = 0x0,
        Y = 0x1,
        Z = 0x2
    };

    namespace base {
        using coord = float;
        using colorRGB = float[3];
        using time = float;
        using accel = float;
        using force = float;
    }

    namespace vec {
        template<typename T>
        struct vec3
        {
            T data[3];

            T& operator[](std::size_t idx)
            {
                return data[idx];
            }

            const T& operator[](std::size_t idx) const
            {
                return data[idx];
            }

        };

        using vec3Coord = vec3<base::coord>;
        using vec3Accel = vec3<base::accel>;
        using vec3Force = vec3<base::force>;
        using vec3f = vec3<float>;

        template<typename T>
        inline vec3<T> operator+ (const vec3<T>& l, const vec3<T>& r) 
        {
            return { l[axis::X] + r[axis::X],
                     l[axis::Y] + r[axis::Y],
                     l[axis::Z] + r[axis::Z] };
        }

        template<typename T>
        inline vec3<T> operator- (const vec3<T>& l, const vec3<T>& r) 
        {
            return { l[axis::X] - r[axis::X],
                     l[axis::Y] - r[axis::Y],
                     l[axis::Z] - r[axis::Z] };
        }

        template<typename T, typename M>
        inline vec3<T> operator* (const vec3<T>& l, const M r) 
        {
            return { l[axis::X] * r,
                     l[axis::Y] * r,
                     l[axis::Z] * r };
        }

        template<typename T, typename M>
        inline vec3<T> operator* (const M l, const vec3<T>& r)
        {
            return { r[axis::X] * l,
                     r[axis::Y] * l,
                     r[axis::Z] * l };
        }
    }
}

#endif
