#include <cstdio>
#include <chrono>
#include <vector>
#include <cstring>
#include <cstdint>

#include "Camera.h"
#include "Config.h"
#include "Renderer.h"
#include "Application.h"

Application::Application(int width, int height) noexcept
    : m_ScreenWidth(width), m_ScreenHeight(height)
{
    m_Camera.position = Vector3{ 250.0f, 1900.0f, 3350.0f };
    m_Camera.target = Vector3{ 1700.0f, 350.0f, 140.0f };
    m_Camera.up = Vector3{ 0.0f, 1.0f, 0.0f };
    m_Camera.fovy = 60;
    m_Camera.projection = CAMERA_PERSPECTIVE;

    m_Bodies.emplace_back(0.0, 0.0, Physics::Const::SUN_MASS, Physics::Const::SUN_RADIUS, Physics::Const::SUN_INCLINE, "Sun", YELLOW);
    m_Bodies.emplace_back(Physics::Const::EARTH_SUN_DISTANCE, -Physics::Const::EARTH_SPEED, Physics::Const::EARTH_MASS, Physics::Const::EARTH_RADIUS, Physics::Const::EARTH_INCLINE, "Earth", BLUE);
    m_Bodies.emplace_back(Physics::Const::JUPTIER_SUN_DISTANCE, -Physics::Const::JUPTIER_SPEED, Physics::Const::JUPITER_MASS, Physics::Const::JUPITER_RADIUS, Physics::Const::JUPTIER_INCLINE, "Jupiter", BROWN);
    m_Bodies.emplace_back(Physics::Const::MERCURY_SUN_DISTANCE, -Physics::Const::MERCURY_SPEED, Physics::Const::MERCURY_MASS, Physics::Const::MERCURY_RADIUS, Physics::Const::MERCURY_INCLINE, "Mercury", GRAY);
    m_Bodies.emplace_back(Physics::Const::VENUS_SUN_DISTANCE, -Physics::Const::VENUS_SPEED, Physics::Const::VENUS_MASS, Physics::Const::VENUS_RADIUS, Physics::Const::VENUS_INCLINE, "Venus", RED);
    m_Bodies.emplace_back(Physics::Const::MARS_SUN_DISTANCE, -Physics::Const::MARS_SPEED, Physics::Const::MARS_MASS, Physics::Const::MARS_RADIUS, Physics::Const::MARS_INCLINE, "Mars", ORANGE);
    m_Bodies.emplace_back(Physics::Const::SATURN_SUN_DISTANCE, -Physics::Const::SATURN_SPEED, Physics::Const::SATURN_MASS, Physics::Const::SATURN_RADIUS, Physics::Const::SATURN_INCLINE, "Saturn", VIOLET);
    m_Bodies.emplace_back(Physics::Const::URANUS_SUN_DISTANCE, -Physics::Const::URANUS_SPEED, Physics::Const::URANUS_MASS, Physics::Const::URANUS_RADIUS, Physics::Const::URANUS_INCLINE, "Uranus", SKYBLUE);
    m_Bodies.emplace_back(Physics::Const::NEPTUN_SUN_DISTANCE, -Physics::Const::NEPTUN_SPEED, Physics::Const::NEPTUN_MASS, Physics::Const::NEPTUN_RADIUS, Physics::Const::NEPTUN_INCLINE, "Neptun", DARKBLUE);

    m_SelectedBody = nullptr;
    m_InfoTimer = std::chrono::steady_clock::now();
}

constexpr int Application::ScreenWidth() const noexcept
{
    return m_ScreenWidth;
}

constexpr int Application::ScreenHeight() const noexcept
{
    return m_ScreenHeight;
}

void Application::SetScreenSize(int width, int height) noexcept
{
    m_ScreenWidth = width;
    m_ScreenHeight = height;
}


void Application::Simulate(float dt)
{
    const auto start = std::chrono::high_resolution_clock::now();
    if (dt < 0.1f) // We need atleast 10 FPS to simulate properly
    {
        //Physics::VelocityVerlet(&m_Bodies, TIME_STEP, dt);
        //Physics::RungeKutta4th(&m_Bodies, TIME_STEP, dt);
        Physics::EulerIntegration(&m_Bodies, TIME_STEP * m_SettingsWindow.GetSimulationRate(), dt);
    }
    const auto end = std::chrono::high_resolution_clock::now();
    m_SimulationTime = std::chrono::duration<double, std::milli>(end - start).count();
}


void Application::OnUpdate(float dt) noexcept
{
    if (m_ShowInfoText)
    {
        const auto endInfoTimer = std::chrono::steady_clock::now();
        const double infoTime = std::chrono::duration<double, std::milli>(endInfoTimer - m_InfoTimer).count();
        if (infoTime > 5000) // 5 secs
            m_ShowInfoText = false;
    }

    if (!m_SettingsWindow.Visible())
        UpdateCameraOverride(&m_Camera, CAMERA_FREE);

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        // ray cast check if player clicked on a planet
        const Vector2 center = { ScreenWidth() / 2.0f, ScreenHeight() / 2.0f };
        const Ray ray = GetScreenToWorldRay(center, m_Camera);

        for (size_t i = 0; i < m_Bodies.size(); i++)
        {
            const Vector3 pos = m_Bodies[i].GetRenderPos();
            const float radius = static_cast<float>((m_Bodies[i].GetRadius() / m_SettingsWindow.GetRenderRadiusScale()) + 30);

            const RayCollision collision = GetRayCollisionSphere(ray, pos, radius);
            if (collision.hit)
            {
                m_SelectedBody = &m_Bodies[i];
                break; // only one even if multiple are hit due to increased hitbox
            }
            else
                m_SelectedBody = nullptr;
        }
    }

    m_ElapsedTime += TIME_STEP * m_SettingsWindow.GetSimulationRate() * dt;
}


// We can't let renderer do this because we need to modify the render positions of the bodies
void Application::RenderPlanets(std::vector<Physics::RigidBody<FLOAT>>* bodies, const Physics::RigidBody<FLOAT>& sun) const
{
    std::vector<Physics::RigidBody<FLOAT>>& bodiesRef = *bodies;

    for (size_t i = 0; i < bodiesRef.size(); i++)
    {
        Vector3 pos = Renderer::MetersToWorld(bodiesRef[i].GetPosition().ToRaylibVector(), m_SettingsWindow.GetRenderDistanceScale());
        const float renderedRadius = (float)(bodiesRef[i].GetRadius() / m_SettingsWindow.GetRenderRadiusScale());

        // Quick and dirty fix to add the radius off the planet and the sun to it's position to
        // properly render it
        if (i != 0)
        {
            Vector3 direction = Vector3Normalize(pos);
            pos = Vector3Add(pos, Vector3Scale(direction, renderedRadius)); // move forward by its rendered radius
            pos = Vector3Add(pos, Vector3Scale(direction, (float)(sun.GetRadius() / m_SettingsWindow.GetRenderRadiusScale())));
        }
        bodiesRef[i].SetRenderPos(pos);
        DrawSphere(pos, renderedRadius, bodiesRef[i].GetColor());
    }
}


void Application::OnRender()
{
    BeginDrawing();
    ClearBackground(BLACK);
    BeginMode3D(m_Camera);

    Renderer::Draw3DGridWithAxes(100, 30.0f);
    RenderPlanets(&m_Bodies, m_Bodies[0]);


    //DrawLine3D(MetersToWorld(earth.GetPosition().ToRaylibVector()), MetersToWorld(moonA.GetPosition().ToRaylibVector()), RED);
    //DrawLine3D(MetersToWorld(earth.GetPosition().ToRaylibVector()), MetersToWorld(moonB.GetPosition().ToRaylibVector()), RED);

    // Optional: show barycenter
    //Vector3 bary = MetersToWorld(Physics::ComputeBarycenter(bodies, 3).ToRaylibVector());
    //DrawSphere(bary, 10.0f, RED);
    EndMode3D();

    Renderer::RenderCoordinateAxis(m_Camera);
    Renderer::RenderPlanetLabels(m_Bodies, m_Camera, m_SettingsWindow.GetRenderRadiusScale());
    Renderer::RenderStats(m_ElapsedTime, m_ShowInfoText, m_SimulationTime, ScreenWidth());
    Renderer::RenderPlanetStats(m_SelectedBody);
    m_SettingsWindow.Draw();

    DrawCircle(ScreenWidth() / 2, ScreenHeight() / 2, 1, WHITE);
    EndDrawing();
}