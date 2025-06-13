#pragma once
#include <cmath>
#include <type_traits>

#include "raylib.h"

namespace Math
{
    template <typename T>
    T Sqrt(T x) noexcept
    {
        if constexpr (std::is_same_v<T, float>)
            return std::sqrtf(x);
        else if constexpr (std::is_same_v<T, double>)
            return std::sqrt(x);
        else if constexpr (std::is_same_v<T, long double>)
            return std::sqrtl(x);
        else
            static_assert(std::is_floating_point_v<T>, "Unsupported type for Math::Sqrt");
    }

    template <typename T>
    class Vector3
    {
    private:
        static_assert(std::is_floating_point_v<T>, "Unsupported type for Math::Vector3");
    public:
        T x;
        T y;
        T z;
    public:
        constexpr Vector3() noexcept : x(static_cast<T>(0)), y(static_cast<T>(0)), z(static_cast<T>(0)) {}
        constexpr Vector3(T x, T y, T z) noexcept : x(x), y(y), z(z) {}

        Vector3& operator=(const Vector3& other) noexcept
        {
            x = other.x;
            y = other.y;
            z = other.z;
            return *this;
        }

        T Length() const noexcept
        {
            return Sqrt<T>(x * x + y * y + z * z);
        }

        Vector3& Normalize() noexcept
        {
            T length = Length();
            if (length != static_cast<T>(0))
            {
                T ilength = static_cast<T>(1) / length;
                x *= ilength;
                y *= ilength;
                z *= ilength;
            }

            return *this;
        }

        T Distance(const Vector3& other) const noexcept
        {
            const Vector3 result = other - *this;
            return result.Length();
        }
        
        Vector3 operator*(T scalar) const noexcept
        {
            return Vector3(x * scalar, y * scalar, z * scalar);
        }

        Vector3 operator-(const Vector3& other) const noexcept
        {
            return Vector3(x - other.x, y - other.y, z - other.z);
        }

        Vector3 operator+(const Vector3& other) const noexcept
        {
            return Vector3(x + other.x, y + other.y, z + other.z);
        }

        Vector3& operator+=(const Vector3& other) noexcept
        {
            x += other.x;
            y += other.y;
            z += other.z;
            return *this;
        }

        constexpr Vector3 operator/(T scalar) const noexcept
        {
            return Vector3( x / scalar, y / scalar, z / scalar);
        }

        ::Vector3 ToRaylibVector() const noexcept
        {
            ::Vector3 r;
            r.x = static_cast<float>(x);
            r.y = static_cast<float>(y);
            r.z = static_cast<float>(z);
            return r;
        }
    };
    using Vector3d = Math::Vector3<double>;
}