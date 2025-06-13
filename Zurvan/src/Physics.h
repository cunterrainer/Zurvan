#pragma once

#include "Math.h"

namespace Physics
{
    namespace Const
    {
        const double G = 6.67430e-11; // m^3 / (kg * s^2)

        /*
            Mass in KG
            Sun distance in meters
            Speed (around the sun) in meters per second
        */
        constexpr double SUN_MASS = 1.988416e30;
        constexpr double EARTH_MASS = 5.972e24;
        constexpr double EARTH_SUN_DISTANCE = 149000000000;
        constexpr double EARTH_SPEED = 29722.2222;

        constexpr double JUPITER_MASS = 1.89813e27;
        constexpr double JUPTIER_SPEED = 13000;
        constexpr double JUPTIER_SUN_DISTANCE = 778000000000;

        constexpr double MERCURY_MASS = 3.30104e23;
        constexpr double MERCURY_SPEED = 47870;
        constexpr double MERCURY_SUN_DISTANCE = 58000000000;

        constexpr double VENUS_MASS = 4.867e24;
        constexpr double VENUS_SPEED = 35000;
        constexpr double VENUS_SUN_DISTANCE = 108000000000;

        constexpr double MARS_MASS = 6.39e23;
        constexpr double MARS_SPEED = 24100;
        constexpr double MARS_SUN_DISTANCE = 227900000000;

        constexpr double SATURN_MASS = 5.683e26;
        constexpr double SATURN_SPEED = 9672;
        constexpr double SATURN_SUN_DISTANCE = 1400000000000;

        constexpr double URANUS_MASS = 8.681e25;
        constexpr double URANUS_SPEED = 6835;
        constexpr double URANUS_SUN_DISTANCE = 2900000000000;

        constexpr double NEPTUN_MASS = 1.024e26;
        constexpr double NEPTUN_SPEED = 5430;
        constexpr double NEPTUN_SUN_DISTANCE = 4500000000000;
    };


    template <typename T>
    struct RigidBody
    {
    private:
        Math::Vector3<T> m_Position;
        Math::Vector3<T> m_Velocity;
        T m_Mass = static_cast<T>(0);
        Color color; // TODO: remove
    public:
        Math::Vector3<T> ComputeAcceleration(const RigidBody& other) const noexcept
        {
            Math::Vector3<T> direction = other.m_Position - m_Position;
            T distance = direction.Length();
            if (distance < static_cast<T>(1)) distance = static_cast<T>(1);

            const T force = static_cast<T>(Const::G * other.m_Mass) / (distance * distance);
            direction = direction.Normalize() * force;
            return direction;
        }

        void SetPosition(T x, T y, T z) noexcept
        {
            m_Position = Math::Vector3<T>(x, y, z);
        }

        void SetPosition(const Math::Vector3<T>& v) noexcept
        {
            m_Position = v;
        }

        void SetVelocity(T x, T y, T z) noexcept
        {
            m_Velocity = Math::Vector3<T>(x, y, z);
        }

        void SetVelocity(const Math::Vector3<T>& v) noexcept
        {
            m_Velocity = v;
        }

        void SetMass(T m) noexcept
        {
            m_Mass = m;
        }

        constexpr const Math::Vector3<T>& GetVelocity() const noexcept
        {
            return m_Velocity;
        }

        constexpr const Math::Vector3<T>& GetPosition() const noexcept
        {
            return m_Position;
        }

        constexpr T GetMass() const noexcept
        {
            return m_Mass;
        }

        void SetColor(Color c)
        {
            color = c;
        }

        Color GetColor()
        {
            return color;
        }
    };


    template <typename T>
    Math::Vector3<T> ComputeBarycenter(Physics::RigidBody<T>* bodies[], int count)
    {
        T totalMass = static_cast<T>(0);
        Math::Vector3<T> weighted;

        for (int i = 0; i < count; i++)
        {
            totalMass += bodies[i]->GetMass();
            weighted += bodies[i]->GetPosition() * bodies[i]->GetMass();
        }

        return weighted * (static_cast<T>(1) / totalMass);
    }
}