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
    const double TIME_STEP = 60 * 60; // 1 hour per second
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
    ~Application() noexcept = default;

    constexpr int ScreenWidth() const noexcept;
    constexpr int ScreenHeight() const noexcept;
    void SetScreenSize(int width, int height) noexcept;

    void Simulate(float dt);
    void OnUpdate(float dt) noexcept;
    void RenderPlanets(std::vector<Physics::RigidBody<FLOAT>>* bodies, const Physics::RigidBody<FLOAT>& sun) const;
    void OnRender();
};