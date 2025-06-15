#include <cstdio>
#include <chrono>
#include <vector>
#include <cstring>
#include <cstdint>

#include "Camera.h"
#include "Config.h"
#include "Renderer.h"
#include "Application.h"

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(*x))

Application::Application(int width, int height) noexcept
    : m_ScreenWidth(width), m_ScreenHeight(height)
{
    m_SettingsWindow.Init();

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

Application::~Application() noexcept
{
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
        Physics::EulerIntegration(&m_Bodies, TIME_STEP, dt);
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
            const float radius = static_cast<float>((m_Bodies[i].GetRadius() / Globals::RADIUS_SCALE) + 30);

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

    m_ElapsedTime += TIME_STEP * dt;
}


void Application::Draw3DGridWithAxes(int size, float spacing) const noexcept
{
    // Draw grid lines along each axis
    for (int i = -size; i <= size; i++)
    {
        // XZ plane (Y=0)
        DrawLine3D({ i * spacing, 0, -size * spacing }, { i * spacing, 0, size * spacing }, Fade(DARKGRAY, 0.3f));
        DrawLine3D({ -size * spacing, 0, i * spacing }, { size * spacing, 0, i * spacing }, Fade(DARKGRAY, 0.3f));

        //// XY plane (Z=0)
        //DrawLine3D({ i * spacing, -size * spacing, 0 }, { i * spacing, size * spacing, 0 }, Fade(DARKGRAY, 0.3f));
        //DrawLine3D({ -size * spacing, i * spacing, 0 }, { size * spacing, i * spacing, 0 }, Fade(DARKGRAY, 0.3f));
        //
        //// YZ plane (X=0)
        //DrawLine3D({ 0, i * spacing, -size * spacing }, Vector3{ 0, i * spacing, size * spacing }, Fade(DARKGRAY, 0.3f));
        //DrawLine3D({ 0, -size * spacing, i * spacing }, Vector3{ 0, size * spacing, i * spacing }, Fade(DARKGRAY, 0.3f));
    }

    // Draw axis lines
    DrawLine3D({ 0, 0, 0 }, { 100, 0, 0 }, RED);   // X axis
    DrawLine3D({ 0, 0, 0 }, { 0, 100, 0 }, GREEN); // Y axis
    DrawLine3D({ 0, 0, 0 }, { 0, 0, 100 }, BLUE);  // Z axis

    // Draw origin
    //DrawSphere(Vector3{ 0, 0, 0 }, 1.0f, YELLOW);
}


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


void Application::RenderPlanetStats() const noexcept
{
    if (m_SelectedBody != nullptr)
    {
        char text[64];
        constexpr std::size_t textSize = ARRAY_SIZE(text);
        std::snprintf(text, textSize, "Name: %s", m_SelectedBody->GetLabel());
        Renderer::DrawText(text, 10, 100);

        std::snprintf(text, textSize, "Position X: %.f", m_SelectedBody->GetPosition().x);
        Renderer::DrawText(text, 10, 120);

        std::snprintf(text, textSize, "Position Y: %.f", m_SelectedBody->GetPosition().y);
        Renderer::DrawText(text, 10, 140);

        std::snprintf(text, textSize, "Position Z: %.f", m_SelectedBody->GetPosition().z);
        Renderer::DrawText(text, 10, 160);

        std::snprintf(text, textSize, "Velocity X: %.f M/S", m_SelectedBody->GetVelocity().x);
        Renderer::DrawText(text, 10, 200);

        std::snprintf(text, textSize, "Velocity Y: %.f M/S", m_SelectedBody->GetVelocity().y);
        Renderer::DrawText(text, 10, 220);

        std::snprintf(text, textSize, "Velocity Z: %.f M/S", m_SelectedBody->GetVelocity().z);
        Renderer::DrawText(text, 10, 240);

        std::snprintf(text, textSize, "Mass: %e KG", m_SelectedBody->GetMass());
        Renderer::DrawText(text, 10, 280);

        const double dist = m_SelectedBody->GetPosition().Distance({ 0, 0, 0 });
        if (dist != 0.0)
        {
            const double angle = std::asin(m_SelectedBody->GetPosition().y / dist);
            std::snprintf(text, textSize, "Inclination: %.4f Radians %.2f Degrees", angle, angle * (180 / Physics::Const::Pi));
            Renderer::DrawText(text, 10, 300);
        }
    }
}


void Application::RenderStats() const noexcept
{
    const double daysPassed = m_ElapsedTime / (60.0 * 60.0 * 24.0);  // seconds to days

    char text[64];
    const std::size_t textSize = ARRAY_SIZE(text);

    std::snprintf(text, textSize, "%d FPS", GetFPS());
    Renderer::DrawText(text, 10, 10);

    std::snprintf(text, textSize, "Days passed: %.2f", daysPassed);
    Renderer::DrawText(text, 10, 40);

    std::snprintf(text, textSize, "Simulation time: %.4f ms", m_SimulationTime);
    Renderer::DrawText(text, 10, 60);

    if (m_ShowInfoText)
    {
        std::strncpy(text, "Press F1 to open the settings window", textSize);
        Renderer::DrawText(text, (ScreenWidth() - MeasureText(text, 20)) / 2, 10);
    }
}


void Application::RenderCoordinateAxis()
{
    // Example positions from world space
    Vector2 screenX = GetWorldToScreen(Vector3{ 105, 0, 0 }, m_Camera);
    Vector2 screenY = GetWorldToScreen(Vector3{ 0, 105, 0 }, m_Camera);
    Vector2 screenZ = GetWorldToScreen(Vector3{ 0, 0, 105 }, m_Camera);

    // Dynamic font size
    int fontSize = 20;
    // Get text width/height for centering
    int widthX = MeasureText("X", fontSize);
    int widthY = MeasureText("Y", fontSize);
    int widthZ = MeasureText("Z", fontSize);

    // Draw centered labels
    Renderer::DrawText("X", screenX.x - widthX / 2, screenX.y - fontSize / 2, RED);
    Renderer::DrawText("Y", screenY.x - widthY / 2, screenY.y - fontSize / 2, GREEN);
    Renderer::DrawText("Z", screenZ.x - widthZ / 2, screenZ.y - fontSize / 2, BLUE);


    for (size_t i = 0; i < m_Bodies.size(); i++)
    {
        // Get their 3D position
        //Vector3 pos = MetersToWorld(bodies[i]->GetPosition().ToRaylibVector());
        Vector3 pos = m_Bodies[i].GetRenderPos();
        pos.y = pos.y + m_Bodies[i].GetRadius() / Globals::RADIUS_SCALE;
        Matrix cameraMatrix = GetCameraMatrix(m_Camera);
        Vector4 cameraSpace = Vector4{ pos.x, pos.y, pos.z, 1.0f } *cameraMatrix;

        if (cameraSpace.z > 0)
            continue;

        // Transform to 2D screen coordinates
        Vector2 screenPos = GetWorldToScreen(pos, m_Camera);

        // Draw label above the sphere
        Renderer::DrawText(
            m_Bodies[i].GetLabel(),
            (int)screenPos.x - MeasureText(m_Bodies[i].GetLabel(), 20) / 2, // center horizontally
            (int)screenPos.y - 20 // move up 20px
        );
    }
}


void Application::OnRender()
{
    BeginDrawing();
    ClearBackground(BLACK);
    BeginMode3D(m_Camera);
    // Draw coordinate axes
    Draw3DGridWithAxes(100, 30.0f);
    RenderPlanets(&m_Bodies, m_Bodies[0]);


    //DrawLine3D(MetersToWorld(earth.GetPosition().ToRaylibVector()), MetersToWorld(moonA.GetPosition().ToRaylibVector()), RED);
    //DrawLine3D(MetersToWorld(earth.GetPosition().ToRaylibVector()), MetersToWorld(moonB.GetPosition().ToRaylibVector()), RED);

    // Optional: show barycenter
    //Vector3 bary = MetersToWorld(Physics::ComputeBarycenter(bodies, 3).ToRaylibVector());
    //DrawSphere(bary, 10.0f, RED);
    EndMode3D();

    RenderCoordinateAxis();
    RenderStats();
    RenderPlanetStats();
    m_SettingsWindow.Draw();


    DrawCircle(ScreenWidth() / 2, ScreenHeight() / 2, 1, WHITE);
    EndDrawing();
}