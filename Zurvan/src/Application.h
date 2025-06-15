#pragma once
#include <vector>
#include <chrono>
#include <cstring>

#include "raylib.h"

#include "GUI.h"
#include "Config.h"
#include "Physics.h"

class Application
{
private:
    #define TIME_SCALAR 1000.0
    const double TIME_STEP = 60 * 60 * TIME_SCALAR;        // 1 hour per frame
private:
    int m_ScreenWidth;
    int m_ScreenHeight;
    bool m_ShowInfoText = true;
    double m_ElapsedTime = 0.0;
    double m_SimulationTime = 0.0;

    Camera3D m_Camera;
    SettingsWindow m_SettingsWindow;
    Physics::RigidBody<FLOAT>* m_SelectedBody;
    std::vector<Physics::RigidBody<FLOAT>> m_Bodies;
    std::chrono::steady_clock::time_point m_InfoTimer;
public:
    Application(int width, int height) noexcept;
    ~Application() noexcept;

    constexpr int ScreenWidth() const noexcept;
    constexpr int ScreenHeight() const noexcept;
    void SetScreenSize(int width, int height) noexcept;

    void Simulate(float dt);
    void OnUpdate(float dt) noexcept;
    void Draw3DGridWithAxes(int size, float spacing) const noexcept;
    void RenderPlanets(std::vector<Physics::RigidBody<FLOAT>>* bodies, const Physics::RigidBody<FLOAT>& sun) const;
    void RenderPlanetStats() const noexcept;
    void RenderStats() const noexcept;
    void RenderCoordinateAxis();
    void OnRender();
};