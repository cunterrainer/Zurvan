#pragma once

#include "Math.h"
#include "Config.h"

namespace Physics
{
    namespace Const
    {
        constexpr FLOAT G = static_cast<FLOAT>(6.67430e-11); // m^3 / (kg * s^2)
        constexpr FLOAT Pi = static_cast<FLOAT>(3.14159265358979323846f);

        /*
            Mass in KG
            Sun distance in meters
            Speed (around the sun) in meters per second
            Incline (approximate orbital inclinations of the major planets relative to the ecliptic plane (the Earth's orbital plane)) in radians
            Radius in meters
        */
        constexpr FLOAT SUN_MASS             = static_cast<FLOAT>(1.988416e30);
        constexpr FLOAT SUN_INCLINE          = static_cast<FLOAT>(0);
        constexpr FLOAT SUN_RADIUS           = static_cast<FLOAT>(696265000);

        constexpr FLOAT EARTH_MASS           = static_cast<FLOAT>(5.972e24);
        constexpr FLOAT EARTH_SUN_DISTANCE   = static_cast<FLOAT>(149000000000);
        constexpr FLOAT EARTH_SPEED          = static_cast<FLOAT>(29722.2222);
        constexpr FLOAT EARTH_INCLINE        = static_cast<FLOAT>(0);
        constexpr FLOAT EARTH_RADIUS         = static_cast<FLOAT>(6378000);

        constexpr FLOAT JUPITER_MASS         = static_cast<FLOAT>(1.89813e27);
        constexpr FLOAT JUPTIER_SPEED        = static_cast<FLOAT>(13000);
        constexpr FLOAT JUPTIER_SUN_DISTANCE = static_cast<FLOAT>(778000000000);
        constexpr FLOAT JUPTIER_INCLINE      = static_cast<FLOAT>(0.023);
        constexpr FLOAT JUPITER_RADIUS       = static_cast<FLOAT>(69911000);

        constexpr FLOAT MERCURY_MASS         = static_cast<FLOAT>(3.30104e23);
        constexpr FLOAT MERCURY_SPEED        = static_cast<FLOAT>(47870);
        constexpr FLOAT MERCURY_SUN_DISTANCE = static_cast<FLOAT>(58000000000);
        constexpr FLOAT MERCURY_INCLINE      = static_cast<FLOAT>(0.122);
        constexpr FLOAT MERCURY_RADIUS       = static_cast<FLOAT>(2439700);

        constexpr FLOAT VENUS_MASS           = static_cast<FLOAT>(4.867e24);
        constexpr FLOAT VENUS_SPEED          = static_cast<FLOAT>(35000);
        constexpr FLOAT VENUS_SUN_DISTANCE   = static_cast<FLOAT>(108000000000);
        constexpr FLOAT VENUS_INCLINE        = static_cast<FLOAT>(0.059);
        constexpr FLOAT VENUS_RADIUS         = static_cast<FLOAT>(6051800);

        constexpr FLOAT MARS_MASS            = static_cast<FLOAT>(6.39e23);
        constexpr FLOAT MARS_SPEED           = static_cast<FLOAT>(24100);
        constexpr FLOAT MARS_SUN_DISTANCE    = static_cast<FLOAT>(227900000000);
        constexpr FLOAT MARS_INCLINE         = static_cast<FLOAT>(0.032);
        constexpr FLOAT MARS_RADIUS          = static_cast<FLOAT>(3389500);

        constexpr FLOAT SATURN_MASS          = static_cast<FLOAT>(5.683e26);
        constexpr FLOAT SATURN_SPEED         = static_cast<FLOAT>(9672);
        constexpr FLOAT SATURN_SUN_DISTANCE  = static_cast<FLOAT>(1400000000000);
        constexpr FLOAT SATURN_INCLINE       = static_cast<FLOAT>(0.044);
        constexpr FLOAT SATURN_RADIUS        = static_cast<FLOAT>(58232000);

        constexpr FLOAT URANUS_MASS          = static_cast<FLOAT>(8.681e25);
        constexpr FLOAT URANUS_SPEED         = static_cast<FLOAT>(6835);
        constexpr FLOAT URANUS_SUN_DISTANCE  = static_cast<FLOAT>(2900000000000);
        constexpr FLOAT URANUS_INCLINE       = static_cast<FLOAT>(0.013);
        constexpr FLOAT URANUS_RADIUS        = static_cast<FLOAT>(25362000);

        constexpr FLOAT NEPTUN_MASS          = static_cast<FLOAT>(1.024e26);
        constexpr FLOAT NEPTUN_SPEED         = static_cast<FLOAT>(5430);
        constexpr FLOAT NEPTUN_SUN_DISTANCE  = static_cast<FLOAT>(4500000000000);
        constexpr FLOAT NEPTUN_INCLINE       = static_cast<FLOAT>(0.031);
        constexpr FLOAT NEPTUN_RADIUS        = static_cast<FLOAT>(24622000);

        constexpr FLOAT PLUTO_MASS = static_cast<FLOAT>(1.303e22);
        constexpr FLOAT PLUTO_SPEED = static_cast<FLOAT>(4748);
        constexpr FLOAT PLUTO_SUN_DISTANCE = static_cast<FLOAT>(5900000000000);
        constexpr FLOAT PLUTO_INCLINE = static_cast<FLOAT>(0.2994985);
        constexpr FLOAT PLUTO_RADIUS = static_cast<FLOAT>(1188000);

        constexpr FLOAT MOON_MASS = 7.347e22; // kg
        constexpr FLOAT MOON_EARTH_DISTANCE = 384400000; // meters
        constexpr FLOAT MOON_SPEED = 1022; // meters per second
        constexpr FLOAT MOON_INCLINE = 0.0872665;
        constexpr FLOAT MOON_RADIUS = static_cast<FLOAT>(1737000);
    };


    enum class SimulationAlgorithm
    {
        EulerIntegration,
        VerletAlgorithm,
        RungeKutta
    };


    template <typename T>
    class RigidBody
    {
    private:
        Math::Vector3<T> m_Position; // using custom Vector3 hence we're able to use doubles instead of floats
        Math::Vector3<T> m_Velocity;
        T m_Mass = static_cast<T>(0);

        // Just for the visuals, not effect on the simulation
        Vector3 m_RenderPosition; // using raylibs Vector3 because it's only for visuals, will be set on the first time rendering
        double m_Radius;
        const char* m_Label;
        Color m_Color;
    public:
        RigidBody() = default;

        RigidBody(T distanceToCenter, T velocityAroundCenter, T mass, double radius, double inclination, const char* name, Color color)
            : m_Mass(mass), m_Radius(radius), m_Label(name), m_Color(color)
        {
            m_Position.x = distanceToCenter;
            m_Position.y = distanceToCenter * static_cast<FLOAT>(std::sin(inclination));
            m_Position.z = 0;

            m_Velocity.x = 0;
            m_Velocity.y = velocityAroundCenter * static_cast<FLOAT>(std::sin(inclination));
            m_Velocity.z = velocityAroundCenter * static_cast<FLOAT>(std::cos(inclination));
        }

        Math::Vector3<T> ComputeAcceleration(const RigidBody& other) const noexcept
        {
            Math::Vector3<T> direction = other.m_Position - m_Position;
            T distance = direction.Length();
            if (distance < static_cast<T>(1)) distance = static_cast<T>(1);

            const T force = static_cast<T>(Const::G * other.m_Mass) / (distance * distance);
            direction = direction.Normalize() * force;
            return direction;
        }

        constexpr void SetRenderPos(const Vector3& pos) noexcept
        {
            m_RenderPosition.x = pos.x;
            m_RenderPosition.y = pos.y;
            m_RenderPosition.z = pos.z;
        }

        constexpr void SetPosition(T x, T y, T z) noexcept
        {
            m_Position = Math::Vector3<T>(x, y, z);
        }

        constexpr void SetPosition(const Math::Vector3<T>& v) noexcept
        {
            m_Position = v;
        }

        constexpr void SetVelocity(T x, T y, T z) noexcept
        {
            m_Velocity = Math::Vector3<T>(x, y, z);
        }

        constexpr void SetVelocity(const Math::Vector3<T>& v) noexcept
        {
            m_Velocity = v;
        }

        constexpr void SetMass(T mass) noexcept
        {
            m_Mass = mass;
        }

        constexpr void SetColor(Color color) noexcept
        {
            m_Color = color;
        }

        constexpr void SetLabel(const char* label) noexcept
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

        constexpr Color GetColor() const noexcept
        {
            return m_Color;
        }

        constexpr Vector3 GetRenderPos() const noexcept
        {
            return m_RenderPosition;
        }

        constexpr double GetRadius() const noexcept
        {
            return m_Radius;
        }

        constexpr const char* GetLabel() const noexcept
        {
            return m_Label;
        }
    };


    template <typename T>
    constexpr Math::Vector3<T> ComputeBarycenter(Physics::RigidBody<T>* bodies[], int count)
    {
        T totalMass = static_cast<T>(0);
        Math::Vector3<T> weighted;

        for (int i = 0; i < count; i++)
        {
            totalMass += bodies[i].GetMass();
            weighted += bodies[i].GetPosition() * bodies[i].GetMass();
        }

        return weighted * (static_cast<T>(1) / totalMass);
    }


    inline void EulerIntegration(std::vector<Physics::RigidBody<FLOAT>>* bodies, double timeStep, float dt) noexcept
    {
        std::vector<Physics::RigidBody<FLOAT>>& bodiesRef = *bodies;

        for (size_t i = 0; i < bodiesRef.size(); ++i)
        {
            Math::Vector3<FLOAT> acc;
            for (size_t k = 0; k < bodiesRef.size(); ++k)
            {
                if (i != k)
                {
                    Math::Vector3<FLOAT> a = bodiesRef[i].ComputeAcceleration(bodiesRef[k]);
                    acc += a;
                }
            }
            bodiesRef[i].SetVelocity(bodiesRef[i].GetVelocity() + (acc * (timeStep * dt)));
            bodiesRef[i].SetPosition(bodiesRef[i].GetPosition() + (bodiesRef[i].GetVelocity() * (timeStep * dt)));
        }
    }


    inline void VelocityVerlet(std::vector<Physics::RigidBody<FLOAT>>* bodies, double timeStep, float delatTime) noexcept
    {
        std::vector<Physics::RigidBody<FLOAT>>& bodiesRef = *bodies;
        std::vector<Math::Vector3<FLOAT>> oldAccelerations(bodiesRef.size());

        // First, compute all initial accelerations
        for (size_t i = 0; i < bodiesRef.size(); ++i)
        {
            Math::Vector3<FLOAT> acc;
            for (size_t j = 0; j < bodiesRef.size(); ++j)
            {
                if (i != j)
                    acc += bodiesRef[i].ComputeAcceleration(bodiesRef[j]);
            }
            oldAccelerations[i] = acc;
        }

        // Now do Velocity Verlet integration
        for (size_t i = 0; i < bodiesRef.size(); ++i)
        {
            const FLOAT dt = static_cast<FLOAT>(timeStep * delatTime);

            auto pos = bodiesRef[i].GetPosition();
            auto vel = bodiesRef[i].GetVelocity();
            auto acc = oldAccelerations[i];

            // Update position
            Math::Vector3<FLOAT> newPos = pos + vel * dt + acc * (0.5 * dt * dt);
            bodiesRef[i].SetPosition(newPos);
        }

        // Recompute accelerations at new positions
        std::vector<Math::Vector3<FLOAT>> newAccelerations(bodiesRef.size());
        for (size_t i = 0; i < bodiesRef.size(); ++i)
        {
            Math::Vector3<FLOAT> acc;
            for (size_t j = 0; j < bodiesRef.size(); ++j)
            {
                if (i != j)
                    acc += bodiesRef[i].ComputeAcceleration(bodiesRef[j]);
            }
            newAccelerations[i] = acc;
        }

        // Update velocities using average of old and new accelerations
        for (size_t i = 0; i < bodiesRef.size(); ++i)
        {
            const FLOAT dt = static_cast<FLOAT>(timeStep * delatTime);

            auto vel = bodiesRef[i].GetVelocity();
            auto accOld = oldAccelerations[i];
            auto accNew = newAccelerations[i];

            Math::Vector3<FLOAT> newVel = vel + (accOld + accNew) * (0.5 * dt);
            bodiesRef[i].SetVelocity(newVel);
        }
    }


    inline void RungeKutta4th(std::vector<Physics::RigidBody<FLOAT>>* bodies, double timeStep, float delatTime)
    {
        std::vector<Physics::RigidBody<FLOAT>>& bodiesRef = *bodies;

        const FLOAT dt = static_cast<FLOAT>(timeStep * delatTime);
        const size_t N = bodiesRef.size();
        
        // Snapshot of initial state
        std::vector<Math::Vector3<FLOAT>> positions(N), velocities(N);
        for (size_t i = 0; i < N; ++i) {
            positions[i] = bodiesRef[i].GetPosition();
            velocities[i] = bodiesRef[i].GetVelocity();
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
                        FLOAT force = static_cast<FLOAT>(Physics::Const::G * bodiesRef[j].GetMass()) / (dist * dist);
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
        
            bodiesRef[i].SetVelocity(newVel);
            bodiesRef[i].SetPosition(newPos);
        }
    }
}