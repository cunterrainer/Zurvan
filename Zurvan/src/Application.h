#pragma once
#include <vector>
#include <utility>

#include "raylib.h"

#include "GUI.h"
#include "Physics.h"
#include "Renderer.h"

class Application
{
private:
    #define TIME_SCALAR 1000.0
    const double TIME_STEP = 60 * 60 * TIME_SCALAR;        // 1 hour per frame
private:
    int m_ScreenWidth = 1280; // TODO remove if only needed for creating the window
    int m_ScreenHeight = 780;
    bool m_ShowInfoText = true;
    double m_ElapsedTime = 0.0;
    double m_SimulationTime = 0.0;
    const char* m_Title = "Zurvan";

    Camera3D m_Camera;
    SettingsWindow m_SettingsWindow;
    std::vector<Physics::RigidBody<FLOAT>> m_Bodies;
    std::chrono::steady_clock::time_point m_InfoTimer;
public:
    Application() noexcept
    {
        InitWindow(m_ScreenWidth, m_ScreenHeight, m_Title);
        SetWindowState(FLAG_WINDOW_RESIZABLE);
        DisableCursor();

        m_SettingsWindow.Init();

        m_Camera.position = Vector3{ 500.0f, 500.0f, 300.0f };
        m_Camera.target = Vector3{ 0.0f, 0.0f, 0.0f };
        m_Camera.up = Vector3{ 0.0f, 1.0f, 0.0f };
        m_Camera.fovy = 45.0f;
        m_Camera.projection = CAMERA_PERSPECTIVE;

        m_Bodies.emplace_back(                                 0.0,                            0.0, Physics::Const::SUN_MASS,     Physics::Const::SUN_RADIUS,     Physics::Const::SUN_INCLINE,     "Sun",     YELLOW);
        m_Bodies.emplace_back(Physics::Const::EARTH_SUN_DISTANCE,   -Physics::Const::EARTH_SPEED,   Physics::Const::EARTH_MASS,   Physics::Const::EARTH_RADIUS,   Physics::Const::EARTH_INCLINE,   "Earth",   BLUE);
        m_Bodies.emplace_back(Physics::Const::JUPTIER_SUN_DISTANCE, -Physics::Const::JUPTIER_SPEED, Physics::Const::JUPITER_MASS, Physics::Const::JUPITER_RADIUS, Physics::Const::JUPTIER_INCLINE, "Jupiter", BROWN);
        m_Bodies.emplace_back(Physics::Const::MERCURY_SUN_DISTANCE, -Physics::Const::MERCURY_SPEED, Physics::Const::MERCURY_MASS, Physics::Const::MERCURY_RADIUS, Physics::Const::MERCURY_INCLINE, "Mercury", GRAY);
        m_Bodies.emplace_back(Physics::Const::VENUS_SUN_DISTANCE,   -Physics::Const::VENUS_SPEED,   Physics::Const::VENUS_MASS,   Physics::Const::VENUS_RADIUS,   Physics::Const::VENUS_INCLINE,   "Venus",   RED);
        m_Bodies.emplace_back(Physics::Const::MARS_SUN_DISTANCE,    -Physics::Const::MARS_SPEED,    Physics::Const::MARS_MASS,    Physics::Const::MARS_RADIUS,    Physics::Const::MARS_INCLINE,    "Mars",    ORANGE);
        m_Bodies.emplace_back(Physics::Const::SATURN_SUN_DISTANCE,  -Physics::Const::SATURN_SPEED,  Physics::Const::SATURN_MASS,  Physics::Const::SATURN_RADIUS,  Physics::Const::SATURN_INCLINE,  "Saturn",  VIOLET);
        m_Bodies.emplace_back(Physics::Const::URANUS_SUN_DISTANCE,  -Physics::Const::URANUS_SPEED,  Physics::Const::URANUS_MASS,  Physics::Const::URANUS_RADIUS,  Physics::Const::URANUS_INCLINE,  "Uranus",  SKYBLUE);
        m_Bodies.emplace_back(Physics::Const::NEPTUN_SUN_DISTANCE,  -Physics::Const::NEPTUN_SPEED,  Physics::Const::NEPTUN_MASS,  Physics::Const::NEPTUN_RADIUS,  Physics::Const::NEPTUN_INCLINE,  "Neptun",  DARKBLUE);
    
        m_InfoTimer = std::chrono::steady_clock::now();
    }

    ~Application() noexcept
    {
        TerminateWindow();
    }

    void Simulate(float dt)
    {
        if (dt < 0.1f) // We need atleast 10 FPS to simulate properly
        {
            //Physics::VelocityVerlet(&m_Bodies, TIME_STEP, dt);
            //Physics::RungeKutta4th(&m_Bodies, TIME_STEP, dt);
            Physics::EulerIntegration(&m_Bodies, TIME_STEP, dt);
        }
    }

    void Run() noexcept
    {
        while (!WindowShouldClose())
        {
            const float dt = GetFrameTime();

            if (m_ShowInfoText)
            {
                auto endInfoTimer = std::chrono::steady_clock::now();
                const double infoTime = std::chrono::duration<double, std::milli>(endInfoTimer - m_InfoTimer).count();
                if (infoTime > 5000) // 5 secs
                    m_ShowInfoText = false;
            }

            auto start = std::chrono::high_resolution_clock::now();
            Simulate(dt);
            auto end = std::chrono::high_resolution_clock::now();
            m_SimulationTime = std::chrono::duration<double, std::milli>(end - start).count();

            if (!m_SettingsWindow.Visible())
                UpdateCameraOverride(&m_Camera, CAMERA_FREE);

            BeginDrawing();
            Render();
            EndDrawing();
        }
    }

    void RenderCoordinateAxis()
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
        DrawText("X", screenX.x - widthX / 2, screenX.y - fontSize / 2, fontSize, RED);
        DrawText("Y", screenY.x - widthY / 2, screenY.y - fontSize / 2, fontSize, GREEN);
        DrawText("Z", screenZ.x - widthZ / 2, screenZ.y - fontSize / 2, fontSize, BLUE);


        for (size_t i = 0; i < m_Bodies.size(); i++)
        {
            // Get their 3D position
            //Vector3 pos = MetersToWorld(bodies[i]->GetPosition().ToRaylibVector());
            Vector3 pos = m_Bodies[i].GetRenderPos();
            Matrix cameraMatrix = GetCameraMatrix(m_Camera);
            Vector4 cameraSpace = Vector4{ pos.x, pos.y, pos.z, 1.0f } *cameraMatrix;

            if (cameraSpace.z > 0)
                continue;

            // Transform to 2D screen coordinates
            Vector2 screenPos = GetWorldToScreen(pos, m_Camera);

            // Draw label above the sphere
            DrawText(
                m_Bodies[i].GetLabel(),
                (int)screenPos.x - MeasureText(m_Bodies[i].GetLabel(), 20) / 2, // center horizontally
                (int)screenPos.y - 20, // move up 20px
                20,
                WHITE
            );
        }
    }


    void Render()
    {
        ClearBackground(BLACK);
        BeginMode3D(m_Camera);
        // Draw coordinate axes
        Renderer::Draw3DGridWithAxes(100, 30.0f);
        Renderer::RenderPlanets(&m_Bodies, m_Bodies[0]);


        //DrawLine3D(MetersToWorld(earth.GetPosition().ToRaylibVector()), MetersToWorld(moonA.GetPosition().ToRaylibVector()), RED);
        //DrawLine3D(MetersToWorld(earth.GetPosition().ToRaylibVector()), MetersToWorld(moonB.GetPosition().ToRaylibVector()), RED);

        // Optional: show barycenter
        //Vector3 bary = MetersToWorld(Physics::ComputeBarycenter(bodies, 3).ToRaylibVector());
        //DrawSphere(bary, 10.0f, RED);
        EndMode3D();

        RenderCoordinateAxis();


        

        m_ElapsedTime += TIME_STEP * GetFrameTime();
        double daysPassed = m_ElapsedTime / (60.0 * 60.0 * 24.0);  // seconds to days
        char dayText[64];
        snprintf(dayText, 64, "Days passed: %.2f", daysPassed);
        DrawText(dayText, 10, 40, 20, WHITE);

        snprintf(dayText, 64, "Simulation time: %.4f ms", m_SimulationTime);
        DrawText(dayText, 10, 60, 20, WHITE);

        if (m_ShowInfoText)
        {
            std::strncpy(dayText, "Press F1 to open the settings window", 64);
            MeasureText(dayText, 20);
            DrawText(dayText, (GetScreenWidth() - MeasureText(dayText, 20)) / 2, 10, 20, WHITE);
        }

        m_SettingsWindow.Draw();

        // Draw Center
        Vector2 center = {
            GetScreenWidth() / 2.0f,
            GetScreenHeight() / 2.0f
        };
        DrawCircle(GetScreenWidth() / 2, GetScreenHeight() / 2, 1, WHITE);
        Ray ray = GetScreenToWorldRay(center, m_Camera);

        static Physics::RigidBody<FLOAT>* stats = nullptr;
        for (size_t i = 0; i < m_Bodies.size(); i++)
        {
            Vector3 pos = m_Bodies[i].GetRenderPos();

            RayCollision collision = GetRayCollisionSphere(ray, pos, (m_Bodies[i].GetRadius() / Renderer::Globals::RADIUS_SCALE) + 30);
            if (collision.hit)
            {
                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                    stats = &m_Bodies[i];
                break; // only one even if multiple are hit due to increased hitbox
            }
            else
                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                    stats = nullptr;
        }

        Renderer::RenderPlanetStats(stats);
    }
};