#pragma once

#include "Math.h"

namespace Physics
{
    namespace Const
    {
        constexpr FLOAT G = static_cast<FLOAT>(6.67430e-11); // m^3 / (kg * s^2)

        /*
            Mass in KG
            Sun distance in meters
            Speed (around the sun) in meters per second
        */
        constexpr FLOAT SUN_MASS = static_cast<FLOAT>(1.988416e30);
        constexpr FLOAT EARTH_MASS = static_cast<FLOAT>(5.972e24);
        constexpr FLOAT EARTH_SUN_DISTANCE = static_cast<FLOAT>(149000000000);
        constexpr FLOAT EARTH_SPEED = static_cast<FLOAT>(29722.2222);

        constexpr FLOAT JUPITER_MASS = static_cast<FLOAT>(1.89813e27);
        constexpr FLOAT JUPTIER_SPEED = static_cast<FLOAT>(13000);
        constexpr FLOAT JUPTIER_SUN_DISTANCE = static_cast<FLOAT>(778000000000);

        constexpr FLOAT MERCURY_MASS = static_cast<FLOAT>(3.30104e23);
        constexpr FLOAT MERCURY_SPEED = static_cast<FLOAT>(47870);
        constexpr FLOAT MERCURY_SUN_DISTANCE = static_cast<FLOAT>(58000000000);

        constexpr FLOAT VENUS_MASS = static_cast<FLOAT>(4.867e24);
        constexpr FLOAT VENUS_SPEED = static_cast<FLOAT>(35000);
        constexpr FLOAT VENUS_SUN_DISTANCE = static_cast<FLOAT>(108000000000);

        constexpr FLOAT MARS_MASS = static_cast<FLOAT>(6.39e23);
        constexpr FLOAT MARS_SPEED = static_cast<FLOAT>(24100);
        constexpr FLOAT MARS_SUN_DISTANCE = static_cast<FLOAT>(227900000000);

        constexpr FLOAT SATURN_MASS = static_cast<FLOAT>(5.683e26);
        constexpr FLOAT SATURN_SPEED = static_cast<FLOAT>(9672);
        constexpr FLOAT SATURN_SUN_DISTANCE = static_cast<FLOAT>(1400000000000);

        constexpr FLOAT URANUS_MASS = static_cast<FLOAT>(8.681e25);
        constexpr FLOAT URANUS_SPEED = static_cast<FLOAT>(6835);
        constexpr FLOAT URANUS_SUN_DISTANCE = static_cast<FLOAT>(2900000000000);

        constexpr FLOAT NEPTUN_MASS = static_cast<FLOAT>(1.024e26);
        constexpr FLOAT NEPTUN_SPEED = static_cast<FLOAT>(5430);
        constexpr FLOAT NEPTUN_SUN_DISTANCE = static_cast<FLOAT>(4500000000000);
    };


    template <typename T>
    struct RigidBody
    {
    private:
        Math::Vector3<T> m_Position;
        Math::Vector3<T> m_Velocity;
        T m_Mass = static_cast<T>(0);
        const char* m_Label = "";
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

        void SetLabel(const char* label) noexcept
        {
            m_Label = label;
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

        constexpr const char* GetLabel() const noexcept
        {
            return m_Label;
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


    template <typename T>
    RigidBody<T> GeneratePlanet(T distanceToSun, T speedAroundSun, T mass, double inclination, const char* name, Color color)
    {
        Physics::RigidBody<T> body;
        const T x = distanceToSun;
        const T y = distanceToSun * static_cast<T>(std::sin(inclination));
        const T z = 0;
        body.SetPosition(x, y, z);

        const T vx = 0;
        const T vy = speedAroundSun * static_cast<T>(std::sin(inclination));
        const T vz = speedAroundSun * static_cast<T>(std::cos(inclination));
        body.SetVelocity(vx, vy, vz);

        body.SetMass(mass);
        body.SetColor(color);
        body.SetLabel(name);
        return body;
    }


    void EulerIntegration(const std::vector<Physics::RigidBody<FLOAT>*>& bodies, double timeStep, float dt) noexcept
    {
        for (size_t i = 0; i < bodies.size(); ++i)
        {
            Math::Vector3<FLOAT> acc;
            for (size_t k = 0; k < bodies.size(); ++k)
            {
                if (i != k)
                {
                    Math::Vector3<FLOAT> a = bodies[i]->ComputeAcceleration(*bodies[k]);
                    acc += a;
                }
            }
            bodies[i]->SetVelocity(bodies[i]->GetVelocity() + (acc * (timeStep * dt)));
            bodies[i]->SetPosition(bodies[i]->GetPosition() + (bodies[i]->GetVelocity() * (timeStep * dt)));
        }
    }


    void VelocityVerlet(const std::vector<Physics::RigidBody<FLOAT>*>& bodies, double timeStep, float delatTime) noexcept
    {
        std::vector<Math::Vector3<FLOAT>> oldAccelerations(bodies.size());

        // First, compute all initial accelerations
        for (size_t i = 0; i < bodies.size(); ++i)
        {
            Math::Vector3<FLOAT> acc;
            for (size_t j = 0; j < bodies.size(); ++j)
            {
                if (i != j)
                    acc += bodies[i]->ComputeAcceleration(*bodies[j]);
            }
            oldAccelerations[i] = acc;
        }

        // Now do Velocity Verlet integration
        for (size_t i = 0; i < bodies.size(); ++i)
        {
            const FLOAT dt = static_cast<FLOAT>(timeStep * delatTime);

            auto pos = bodies[i]->GetPosition();
            auto vel = bodies[i]->GetVelocity();
            auto acc = oldAccelerations[i];

            // Update position
            Math::Vector3<FLOAT> newPos = pos + vel * dt + acc * (0.5 * dt * dt);
            bodies[i]->SetPosition(newPos);
        }

        // Recompute accelerations at new positions
        std::vector<Math::Vector3<FLOAT>> newAccelerations(bodies.size());
        for (size_t i = 0; i < bodies.size(); ++i)
        {
            Math::Vector3<FLOAT> acc;
            for (size_t j = 0; j < bodies.size(); ++j)
            {
                if (i != j)
                    acc += bodies[i]->ComputeAcceleration(*bodies[j]);
            }
            newAccelerations[i] = acc;
        }

        // Update velocities using average of old and new accelerations
        for (size_t i = 0; i < bodies.size(); ++i)
        {
            const FLOAT dt = static_cast<FLOAT>(timeStep * delatTime);

            auto vel = bodies[i]->GetVelocity();
            auto accOld = oldAccelerations[i];
            auto accNew = newAccelerations[i];

            Math::Vector3<FLOAT> newVel = vel + (accOld + accNew) * (0.5 * dt);
            bodies[i]->SetVelocity(newVel);
        }
    }


    void RungeKutta4th(const std::vector<Physics::RigidBody<FLOAT>*>& bodies, double timeStep, float delatTime)
    {
        const FLOAT dt = static_cast<FLOAT>(timeStep * delatTime);
        const size_t N = bodies.size();
        
        // Snapshot of initial state
        std::vector<Math::Vector3<FLOAT>> positions(N), velocities(N);
        for (size_t i = 0; i < N; ++i) {
            positions[i] = bodies[i]->GetPosition();
            velocities[i] = bodies[i]->GetVelocity();
        }
        
        // Helper: Compute all accelerations from positions
        auto ComputeAllAccelerations = [&](const std::vector<Math::Vector3<FLOAT>>& pos) {
            std::vector<Math::Vector3<FLOAT>> accs(N);
            for (size_t i = 0; i < N; ++i) {
                Math::Vector3<FLOAT> acc;
                for (size_t j = 0; j < N; ++j) {
                    if (i != j) {
                        Math::Vector3<FLOAT> dir = pos[j] - pos[i];
                        FLOAT dist = dir.Length();
                        if (dist < 1.0) dist = 1.0;
                        FLOAT force = static_cast<FLOAT>(Physics::Const::G * bodies[j]->GetMass()) / (dist * dist);
                        acc += dir.Normalize() * force;
                    }
                }
                accs[i] = acc;
            }
            return accs;
            };
        
        // --- RK4 Steps ---
        std::vector<Math::Vector3<FLOAT>> k1_v(N), k1_p(N);
        std::vector<Math::Vector3<FLOAT>> k2_v(N), k2_p(N);
        std::vector<Math::Vector3<FLOAT>> k3_v(N), k3_p(N);
        std::vector<Math::Vector3<FLOAT>> k4_v(N), k4_p(N);
        
        // k1
        auto acc1 = ComputeAllAccelerations(positions);
        for (size_t i = 0; i < N; ++i) {
            k1_v[i] = acc1[i] * dt;
            k1_p[i] = velocities[i] * dt;
        }
        
        // k2
        std::vector<Math::Vector3<FLOAT>> pos_k2(N), vel_k2(N);
        for (size_t i = 0; i < N; ++i) {
            pos_k2[i] = positions[i] + k1_p[i] * 0.5f;
            vel_k2[i] = velocities[i] + k1_v[i] * 0.5f;
        }
        auto acc2 = ComputeAllAccelerations(pos_k2);
        for (size_t i = 0; i < N; ++i) {
            k2_v[i] = acc2[i] * dt;
            k2_p[i] = vel_k2[i] * dt;
        }
        
        // k3
        std::vector<Math::Vector3<FLOAT>> pos_k3(N), vel_k3(N);
        for (size_t i = 0; i < N; ++i) {
            pos_k3[i] = positions[i] + k2_p[i] * 0.5f;
            vel_k3[i] = velocities[i] + k2_v[i] * 0.5f;
        }
        auto acc3 = ComputeAllAccelerations(pos_k3);
        for (size_t i = 0; i < N; ++i) {
            k3_v[i] = acc3[i] * dt;
            k3_p[i] = vel_k3[i] * dt;
        }
        
        // k4
        std::vector<Math::Vector3<FLOAT>> pos_k4(N), vel_k4(N);
        for (size_t i = 0; i < N; ++i) {
            pos_k4[i] = positions[i] + k3_p[i];
            vel_k4[i] = velocities[i] + k3_v[i];
        }
        auto acc4 = ComputeAllAccelerations(pos_k4);
        for (size_t i = 0; i < N; ++i) {
            k4_v[i] = acc4[i] * dt;
            k4_p[i] = vel_k4[i] * dt;
        }
        
        // Final update
        for (size_t i = 0; i < N; ++i) {
            Math::Vector3<FLOAT> newVel = velocities[i] + (k1_v[i] + k2_v[i] * 2.0 + k3_v[i] * 2.0 + k4_v[i]) / 6.0;
            Math::Vector3<FLOAT> newPos = positions[i] + (k1_p[i] + k2_p[i] * 2.0 + k3_p[i] * 2.0 + k4_p[i]) / 6.0;
        
            bodies[i]->SetVelocity(newVel);
            bodies[i]->SetPosition(newPos);
        }
    }
}