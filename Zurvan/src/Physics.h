#pragma once

#include "Math.h"

namespace Physics
{
    namespace Const
    {
        const double G = 6.67430e-11;
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