#pragma once
#include <limits>

namespace math
{
    template <typename T = double>
    constexpr T pi()
    {
        return static_cast<T>(3.14159265358979323846);
    }

    template <typename T>
    constexpr int sgn(T val)
    {
        return (T(0) < val) - (val < T(0));
    }

    template<typename T>
    constexpr T gcd(T a, T b)
    {
        static_assert(std::is_integral_v<T>, "gcd can only handle integers");
        if (b == 0)
            return a;
        return gcd(b, a % b);
    }
    constexpr double infinity = std::numeric_limits<double>::infinity();
}
