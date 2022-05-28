#pragma once
#include "vec.h"

namespace math
{
    template <typename T, size_t w, size_t h>
    struct mat;

    template <typename T, typename T1, size_t h1, size_t w1, size_t w2>
    constexpr auto operator*(const mat<T, w1, h1> &a, const mat<T1, w2, w1> &b);

    template <typename S, typename T, size_t w, size_t h>
    constexpr auto operator*(S scale, const mat<T, w, h> &a);

    template <typename S, typename T, size_t w, size_t h>
    constexpr auto operator*(const mat<T, w, h> &a, S scale);

    template<typename M, size_t w, size_t h, typename V>
    constexpr auto operator*(const mat<M, w, h>& a, const vec<V, w>& b);

    template <typename T, size_t w, size_t h>
    struct mat
    {
        vec<T, h> data[w];

        constexpr auto &operator[](size_t i)
        {
            return data[i];
        }

        constexpr const auto &operator[](size_t i) const
        {
            return data[i];
        }

        template <typename OT>
        constexpr mat &operator=(const mat<OT, w, h> &o)
        {
            for (size_t x = 0; x < w; ++x)
            {
                for (size_t y = 0; y < h; ++y)
                {
                    data[x][y] = static_cast<T>(o[x][y]);
                }
            }
            return *this;
        }

        template <typename OT, size_t ow, size_t oh>
        constexpr mat &operator*=(const mat<OT, ow, oh> &o)
        {
            return *this = *this * o;
        }

        template <typename OT>
        constexpr mat &operator*=(OT o)
        {
            return *this = *this * o;
        }

        constexpr vec<T, w> get_row(size_t row_num) const
        {
            vec<T, w> res;
            for (size_t i = 0; i < w; ++i)
            {
                res[i] = data[i][row_num];
            }
            return res;
        }

        constexpr static size_t size()
        {
            return w;
        }

        constexpr static size_t width()
        {
            return w;
        }

        constexpr static size_t height()
        {
            return h;
        }
    };

    // w1xh1 * w2xw1 --> w2xh1
    template <typename T, typename T1, size_t h1, size_t w1, size_t w2>
    constexpr auto operator*(const mat<T, w1, h1> &a, const mat<T1, w2, w1> &b)
    {
        mat<decltype(float{} * T{} * T1{}), w2, h1> res;
        for (size_t row{}; row < a.height(); ++row)
        {
            auto rowvec = a.get_row(row);
            for (size_t col{}; col < b.width(); ++col)
            {
                res[col][row] = dot(rowvec, b[col]);
            }
        }
        return res;
    }

    template <typename S, typename T, size_t w, size_t h>
    constexpr auto operator*(S scale, const mat<T, w, h> &a)
    {
        mat<decltype(float{} * S{} * T{}), w, h> res;
        for (size_t col{}; col < a.width(); ++col)
        {
            for (size_t row{}; row < a.height(); ++row)
            {
                res[col][row] = a[col][row] * scale;
            }
        }
        return res;
    }

    template <typename S, typename T, size_t w, size_t h>
    constexpr auto operator*(const mat<T, w, h> &a, S scale)
    {
        return scale * a;
    }

    template<typename M, size_t w, size_t h, typename V>
    constexpr auto operator*(const mat<M, w, h>& a, const vec<V, w>& b)
    {
        static_assert(decltype((a * (*reinterpret_cast<const mat<V, 1, w>*>(&b))))::width() == 1, "size mismatch");
        return (a * (*reinterpret_cast<const mat<V, 1, w>*>(&b)))[0];
    }

    template <typename T>
    using to_float = decltype(T{} * float{});

    template <typename T>
    constexpr mat<to_float<T>, 4, 4> ortho_mat(T left, T right, T bottom, T top, T _near, T _far)
    {
        using t = to_float<T>;
        return {
            vec<t, 4>{2.f / (right - left), 0.f, 0.f, 0.f},
            vec<t, 4>{0.f, 2.f / (top - bottom), 0.f, 0.f},
            vec<t, 4>{0.f, 0.f, -2.f / (_far - _near), 0.f},
            vec<t, 4>{-1.f * (right + left) / (right - left), -1.f * (top + bottom) / (top - bottom), -1.f * (_far + _near) / (_far - _near), 1.f},
        };
    }

    template <typename T>
    constexpr mat<to_float<T>, 4, 4> x_rot(T rads)
    {
        using t = to_float<T>;
        return {
            vec<t, 4>{1.f, 0.f, 0.f, 0.f},
            vec<t, 4>{0.f, cosf(rads), sinf(rads), 0.f},
            vec<t, 4>{0.f, -sinf(rads), cosf(rads), 0.f},
            vec<t, 4>{0.f, 0.f, 0.f, 1.f},
        };
    }

    template <typename T>
    constexpr mat<to_float<T>, 4, 4> y_rot(T rads)
    {
        using t = to_float<T>;
        return {
            vec<t, 4>{cosf(rads), 0.f, -sinf(rads), 0.f},
            vec<t, 4>{0.f, 1.f, 0.f, 0.f},
            vec<t, 4>{sinf(rads), 0.f, cosf(rads), 0.f},
            vec<t, 4>{0.f, 0.f, 0.f, 1.f},
        };
    }

    template <typename T>
    constexpr mat<to_float<T>, 4, 4> z_rot(T rads)
    {
        using t = to_float<T>;
        return {
            vec<t, 4>{cosf(rads), sinf(rads), 0.f, 0.f},
            vec<t, 4>{-sinf(rads), cosf(rads), 0.f, 0.f},
            vec<t, 4>{0.f, 0.f, 1.f, 0.f},
            vec<t, 4>{0.f, 0.f, 0.f, 1.f},
        };
    }

    template <typename T = float>
    constexpr mat<to_float<T>, 4, 4> rot(T rads, T x, T y, T z)
    {
        using t = to_float<T>;

        auto c = cosf(rads);
        auto s = sinf(rads);
        return {
            vec<t, 4>{c + x * x * (1 - c), y * x * (1 - c) + z * s, z * x * (1 - c) - y * s, 0.f},
            vec<t, 4>{x * y * (1 - c) - z * s, c + y * y * (1 - c), z * y * (1 - c) + x * s, 0.f},
            vec<t, 4>{x * z * (1 - c) + y * s, y * z * (1 - c) - x * s, c + z * z * (1 - c), 0.f},
            vec<t, 4>{0.f, 0.f, 0.f, 1.f},
        };
    }

    template<typename T = float>
    constexpr mat<to_float<T>, 2, 2> rot2d(T rads)
    {
        using t = to_float<T>;

        auto c = cosf(rads);
        auto s = sinf(rads);
        return {
            vec<t, 2>{c, s},
            vec<t, 2>{-s, c},
        };
    }

    template <typename T = float>
    constexpr mat<to_float<T>, 4, 4> translate(T x, T y, T z)
    {
        using t = to_float<T>;
        return {
            vec<t, 4>{1.f, 0.f, 0.f, 0.f},
            vec<t, 4>{0.f, 1.f, 0.f, 0.f},
            vec<t, 4>{0.f, 0.f, 1.f, 0.f},
            vec<t, 4>{static_cast<t>(x), static_cast<t>(y), static_cast<t>(z), 1.f},
        };
    }

    template <typename T = float>
    constexpr mat<to_float<T>, 4, 4> scale(T x, T y, T z)
    {
        using t = to_float<T>;
        return {
            vec<t, 4>{static_cast<t>(x), 0.f, 0.f, 0.f},
            vec<t, 4>{0.f, static_cast<t>(y), 0.f, 0.f},
            vec<t, 4>{0.f, 0.f, static_cast<t>(z), 0.f},
            vec<t, 4>{0.f, 0.f, 0.f, 1.f},
        };
    }

    template <typename T = float, size_t w = 4>
    constexpr mat<T, w, w> identity()
    {
        mat<T, w, w> res{};
        for (size_t i = 0; i < w; ++i)
        {
            res[i][i] = 1;
        }
        return res;
    }

    template <typename T, size_t w, size_t h>
    const T *value(const mat<T, w, h> &m)
    {
        return reinterpret_cast<const T *>(m.data);
    }
}