#pragma once
#include <tuple>
#include <cmath>

#include "debug.h"

#define vparam(i) typename T##i, size_t sz##i
#define varg(i) vec<T##i, sz##i>
#define vec_ops                                                           \
    constexpr T &operator[](size_t i) { return data[i]; }                 \
    constexpr T operator[](size_t i) const { return data[i]; }            \
    constexpr vec &operator+=(const vec &o) { return *this = *this + o; } \
    constexpr vec &operator-=(const vec &o) { return *this = *this - o; } \
    constexpr vec &operator/=(const vec &o) { return *this = *this / o; } \
    template <typename O>                                                 \
    constexpr vec &operator/=(O o) { return *this = *this / o; }          \
    constexpr vec &operator*=(const vec &o) { return *this = *this * o; } \
    template <typename O>                                                 \
    constexpr vec &operator*=(O o) { return *this = *this * o; }          \
    template <typename T1>                                                \
    constexpr operator vec<T1, size()>() const                            \
    {                                                                     \
        vec<T1, size()> res;                                              \
        for (size_t i = 0; i < size(); ++i)                               \
            res[i] = static_cast<T1>(data[i]);                            \
        return res;                                                       \
    }                                                                     \
    template <typename oT, size_t osz>                                    \
    constexpr vec(const vec<oT, osz> &other) : data{}                     \
    {                                                                     \
        for (size_t i = 0; i < other.size(); ++i)                         \
            data[i] = other[i];                                           \
    }                                                                     \
    template <typename oT, size_t osz>                                    \
    constexpr vec &operator=(const vec<oT, osz> &other)                   \
    {                                                                     \
        for (size_t i = 0; i < other.size(); ++i)                         \
            data[i] = other[i];                                           \
        for (size_t i = other.size(); i < size(); ++i)                    \
            data[i] = {};                                                 \
        return *this;                                                     \
    }

namespace math
{
    template <typename T, size_t sz>
    struct vec;

    template <typename T, size_t w, size_t h>
    struct mat;

    template <typename T, typename T1, size_t sz>
    constexpr auto operator+(const vec<T, sz> &a, const vec<T1, sz> &b);

    template <typename T, typename T1, size_t sz>
    constexpr auto operator-(const vec<T, sz> &a, const vec<T1, sz> &b);

    template <typename T, typename T1, size_t sz>
    constexpr auto operator*(const vec<T, sz> &a, const vec<T1, sz> &b);

    template <typename T, typename T1, size_t sz>
    constexpr auto operator/(const vec<T, sz> &a, const vec<T1, sz> &b);

    template <typename T, size_t sz, typename T1>
    constexpr auto operator*(const vec<T, sz> &a, T1 b);

    template <typename T, size_t sz, typename T1>
    constexpr auto operator*(T a, const vec<T1, sz> &b);

    template <typename T, size_t sz, typename T1>
    constexpr auto operator/(const vec<T, sz> &a, T1 b);

    template <typename T, size_t sz>
    struct vec
    {
        static_assert(sz != 0, "vector size cannot be 0");

        static constexpr size_t size() { return sz; }

        using value_type = T;

        union
        {
            struct
            {
                T x, y, z, w;
            };
            T data[sz];
        };

        constexpr vec() : data{} {}

        template<typename... Ts>
        constexpr vec(Ts... vals) : data{static_cast<T>(vals)...} {}

        constexpr vec(const vec& other) = default;
        constexpr vec& operator=(const vec& other) = default;

        vec_ops
    };

    template <typename T>
    struct vec<T, 1>
    {
        static constexpr size_t size() { return 1; }

        using value_type = T;

        union
        {
            struct
            {
                T x;
            };

            T data[1];
        };
        
        constexpr vec() : data{} {}

        template<typename... Ts>
        constexpr vec(Ts... vals) : data{static_cast<T>(vals)...} {}

        constexpr vec(const vec& other) = default;
        constexpr vec& operator=(const vec& other) = default;

        vec_ops
    };

    template <typename T>
    struct vec<T, 2>
    {
        static constexpr size_t size() { return 2; }

        using value_type = T;

        union
        {
            struct
            {
                T x, y;
            };

            T data[2];
        };

        
        constexpr vec() : data{} {}

        template<typename... Ts>
        constexpr vec(Ts... vals) : data{static_cast<T>(vals)...} {}

        constexpr vec(const vec& other) = default;
        constexpr vec& operator=(const vec& other) = default;

        vec_ops
    };

    template <typename T>
    struct vec<T, 3>
    {
        static constexpr size_t size() { return 3; }

        using value_type = T;

        union
        {
            struct
            {
                T x, y, z;
            };

            T data[3];
        };

        constexpr vec() : data{} {}

        template<typename... Ts>
        constexpr vec(Ts... vals) : data{static_cast<T>(vals)...} {}

        constexpr vec(const vec& other) = default;
        constexpr vec& operator=(const vec& other) = default;

        vec_ops
    };

    namespace vec_help
    {
        template <size_t i, typename T, typename T1, typename TR, size_t sz, typename Op>
        constexpr void set(const vec<T, sz> &a, const vec<T1, sz> &b, vec<TR, sz> &res, Op o)
        {
            res[i] = o(a[i], b[i]);
            if constexpr (i < sz - 1)
                set<i + 1>(a, b, res, o);
        }

        template <size_t i, typename T, typename T1, typename TR, size_t sz, typename Op>
        constexpr void set(const vec<T, sz> &a, T1 b, vec<TR, sz> &res, Op o)
        {
            res[i] = o(a[i], b);
            if constexpr (i < sz - 1)
                set<i + 1>(a, b, res, o);
        }

        template <typename T0, typename T1>
        constexpr auto plus(T0 a, T1 b) -> decltype(T0{} + T1{})
        {
            return a + b;
        }

        template <typename T0, typename T1>
        constexpr auto minus(T0 a, T1 b) -> decltype(T0{} - T1{})
        {
            return a - b;
        }

        template <typename T0, typename T1>
        constexpr auto mult(T0 a, T1 b) -> decltype(T0{} * T1{})
        {
            return a * b;
        }

        template <typename T0, typename T1>
        constexpr auto div(T0 a, T1 b) -> decltype(T0{} / T1{})
        {
            return a / b;
        }
    }

    template <typename T, typename T1, size_t sz>
    constexpr auto operator+(const vec<T, sz> &a, const vec<T1, sz> &b)
    {
        vec<decltype(T{} + T1{}), sz> res;
        vec_help::set<0>(a, b, res, vec_help::plus<T, T1>);
        return res;
    }

    template <typename T, typename T1, size_t sz>
    constexpr auto operator-(const vec<T, sz> &a, const vec<T1, sz> &b)
    {
        vec<decltype(T{} - T1{}), sz> res;
        vec_help::set<0>(a, b, res, vec_help::minus<T, T1>);
        return res;
    }

    template <typename T, typename T1, size_t sz>
    constexpr auto operator*(const vec<T, sz> &a, const vec<T1, sz> &b)
    {
        vec<decltype(T{} + T1{}), sz> res;
        vec_help::set<0>(a, b, res, vec_help::mult<T, T1>);
        return res;
    }

    template <typename T, typename T1, size_t sz>
    constexpr auto operator/(const vec<T, sz> &a, const vec<T1, sz> &b)
    {
        vec<decltype(T{} + T1{}), sz> res;
        vec_help::set<0>(a, b, res, vec_help::div<T, T1>);
        return res;
    }

    template <typename T, size_t sz, typename T1>
    constexpr auto operator*(const vec<T, sz> &a, T1 b)
    {
        vec<decltype(T{} * T1{}), sz> res;
        vec_help::set<0>(a, b, res, vec_help::mult<T, T1>);
        return res;
    }

    template <typename T, size_t sz, typename T1>
    constexpr auto operator*(T a, const vec<T1, sz> &b)
    {
        vec<decltype(T{} * T1{}), sz> res;
        vec_help::set<0>(b, a, res, vec_help::mult<T1, T>);
        return res;
    }

    template <typename T, size_t sz, typename T1>
    constexpr auto operator/(const vec<T, sz> &a, T1 b)
    {
        vec<decltype(T{} / T1{}), sz> res;
        vec_help::set<0>(a, b, res, vec_help::div<T, T1>);
        return res;
    }

    template <typename T, size_t sz>
    constexpr vec<T, sz> operator-(const vec<T, sz> &a)
    {
        return a * -1.f;
    }

    template<typename T, typename T1, size_t sz>
    constexpr bool operator==(const vec<T, sz>& a, const vec<T1, sz>& b)
    {
        for (size_t i = 0; i < sz; ++i)
            if (a[i] != b[i])
                return false;
        return true;
    }

    template<typename T, typename T1, size_t sz>
    constexpr bool operator!=(const vec<T, sz>& a, const vec<T1, sz>& b)
    {
        return !(a == b);
    }

    template <typename T0, size_t sz>
    constexpr double dot(const vec<T0, sz> &a, const vec<T0, sz> &b)
    {
        double res{0};
        for (size_t i = 0; i < sz; ++i)
        {
            res += a[i] * b[i];
        }
        return res;
    }

    template <typename T0, typename T1>
    constexpr vec<decltype(T0{} * T1{}), 3> cross(const vec<T0, 3> &a, const vec<T1, 3> &b)
    {
        return {
            a[1] * b[2] - a[2] * b[1],
            a[2] * b[0] - a[0] * b[2],
            a[0] * b[1] - a[1] * b[0]
        };
    }

    template <vparam(0)>
    constexpr const T0 *value(const varg(0) & a)
    {
        return a.data;
    }

    template<typename T, size_t sz>
    constexpr T magnitude2(const vec<T, sz>& v)
    {
        double res = 0;
        for (size_t i = 0; i < sz; ++i)
            res += v[i] * v[i];
        return res;
    }

    template<typename T, size_t sz>
    constexpr T magnitude(const vec<T, sz>& v)
    {
        return std::sqrt(magnitude2(v));
    }

    template <typename T, size_t sz>
    constexpr vec<T, sz> normalize(const vec<T, sz>& v)
    {
        return v / magnitude(v);
    }

    template <typename T>
    constexpr vec<T, 2> perpindicular(const vec<T, 2> &v)
    {
        return {-v.y, v.x};
    }

    template <typename T>
    constexpr double det(const vec<T, 2> &a, const vec<T, 2> &b)
    {
        return a.x * b.y - b.x * a.y;
    }
    template <typename T>
    constexpr double cross(const vec<T, 2> &a, const vec<T, 2> &b)
    {
        return a.x * b.y - a.y * b.x;
    }

    template <typename T>
    constexpr double perp_dot(const vec<T, 2> &a, const vec<T, 2> &b)
    {
        return cross(a, b);
    }

    template <typename T, size_t sz>
    constexpr T distance(const vec<T, sz> &a, const vec<T, sz> &b)
    {
        return magnitude(a - b);
    }
    template <typename T, size_t sz>
    constexpr T distance2(const vec<T, sz> &a, const vec<T, sz> &b)
    {
        return magnitude2(a - b);
    }
    template <typename T, size_t sz>
    constexpr bool is_in_radius(const vec<T, sz> &a, const vec<T, sz> &b, T r)
    {
        return distance2(a, b) <= r * r;
    }
    template <typename T, size_t sz>
    constexpr double angle(const vec<T, sz> &a, const vec<T, sz> &b)
    {
        return acos(dot(a, b) / a.magnitude() * b.magnitude());
    }

    template<typename T>
    constexpr double angle(const vec<T, 2>& a)
    {
        return atan2(a.y, a.x);
    }

    template<typename T, typename R>
    void rotate(vec<T, 2>& v, R radians, vec<T, 2> center) {
        v -= center;
		vec<T, 2> og = v;

		v.x = og.x * cos(radians) - og.y * sin(radians);
		v.y = og.y * cos(radians) + og.x * sin(radians);
		v += center;
	}

    using vec1 = vec<float, 1>;
    using vec2 = vec<float, 2>;
    using vec3 = vec<float, 3>;
    using vec4 = vec<float, 4>;

    using dvec1 = vec<double, 1>;
    using dvec2 = vec<double, 2>;
    using dvec3 = vec<double, 3>;
    using dvec4 = vec<double, 4>;
}

#ifndef NDEBUG
#include <ostream>
template<typename T, size_t sz>
std::ostream& operator<<(std::ostream& o, const math::vec<T, sz>& v)
{
    o.put('(');
    o << v[0];
    for (size_t i = 1; i < v.size(); ++i)
        o << ", " << v[i];
    return o.put(')');
}
#endif