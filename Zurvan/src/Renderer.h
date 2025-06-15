#pragma once
#include <cmath>
#include <vector>

#include "raylib.h"
#include "raymath.h"

#include "Physics.h"

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(*x))

namespace Globals
{
    inline double DISTANCE_SCALE = 700000000; // 1 px = 700,000,000 meters
    inline double RADIUS_SCALE = 1e6; // 1 px = 1,000,000 meters
}


class Renderer
{
private:
    static inline Font s_RenderFont;
public:
    static constexpr int FontSize = 25;
public:
    static void Init() noexcept
    {
        s_RenderFont = LoadFontEx("res/Fonts/Roboto/static/Roboto-Regular.ttf", FontSize, NULL, 0);
        SetTextureFilter(s_RenderFont.texture, TEXTURE_FILTER_POINT);
    }

    static void Shutdown() noexcept
    {
        UnloadFont(s_RenderFont);
    }

    static Vector3 MetersToWorld(Vector3 meters, float distanceScale) noexcept
    {
        return Vector3{ meters.x / distanceScale, meters.y / distanceScale, meters.z / distanceScale };
    }

    static void DrawText(const char* text, int x, int y, Color color = WHITE) noexcept
    {
        DrawTextEx(s_RenderFont, text, { static_cast<float>(x), static_cast<float>(y)}, FontSize, 0, color);
    }

    static void Draw3DGridWithAxes(int size, float spacing) noexcept
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

    static void RenderPlanetLabels(const std::vector<Physics::RigidBody<FLOAT>>& bodies, const Camera& camera, float renderRadiusScale)
    {
        for (const auto& body : bodies)
        {
            // Get their 3D position
            Vector3 pos = body.GetRenderPos();
            pos.y = pos.y + (float)body.GetRadius() / renderRadiusScale;

            const Matrix cameraMatrix = GetCameraMatrix(camera);
            const Vector4 cameraSpace = Vector4{ pos.x, pos.y, pos.z, 1.0f } * cameraMatrix;

            if (cameraSpace.z > 0) // body not visible
                continue;

            // Transform to 2D screen coordinates
            const Vector2 screenPos = GetWorldToScreen(pos, camera);

            // Draw label above the sphere
            Renderer::DrawText(body.GetLabel(), (int)screenPos.x - MeasureText(body.GetLabel(), Renderer::FontSize) / 2, (int)screenPos.y - 20);
        }
    }

    static void RenderCoordinateAxis(const Camera& camera) noexcept
    {
        // Example positions from world space
        const Vector2 screenX = GetWorldToScreen(Vector3{ 105, 0, 0 }, camera);
        const Vector2 screenY = GetWorldToScreen(Vector3{ 0, 105, 0 }, camera);
        const Vector2 screenZ = GetWorldToScreen(Vector3{ 0, 0, 105 }, camera);

        // Get text width/height for centering
        const int widthX = MeasureText("X", Renderer::FontSize);
        const int widthY = MeasureText("Y", Renderer::FontSize);
        const int widthZ = MeasureText("Z", Renderer::FontSize);

        // Draw centered labels
        Renderer::DrawText("X", (int)screenX.x - widthX / 2, (int)screenX.y - Renderer::FontSize / 2, RED);
        Renderer::DrawText("Y", (int)screenY.x - widthY / 2, (int)screenY.y - Renderer::FontSize / 2, GREEN);
        Renderer::DrawText("Z", (int)screenZ.x - widthZ / 2, (int)screenZ.y - Renderer::FontSize / 2, BLUE);
    }

    static void RenderPlanetStats(const Physics::RigidBody<FLOAT>* const body) noexcept
    {
        if (body != nullptr)
        {
            char text[64];
            constexpr std::size_t textSize = ARRAY_SIZE(text);
            std::snprintf(text, textSize, "Name: %s", body->GetLabel());
            Renderer::DrawText(text, 10, 100);

            std::snprintf(text, textSize, "Position X: %.f", body->GetPosition().x);
            Renderer::DrawText(text, 10, 120);

            std::snprintf(text, textSize, "Position Y: %.f", body->GetPosition().y);
            Renderer::DrawText(text, 10, 140);

            std::snprintf(text, textSize, "Position Z: %.f", body->GetPosition().z);
            Renderer::DrawText(text, 10, 160);

            std::snprintf(text, textSize, "Velocity X: %.f M/S", body->GetVelocity().x);
            Renderer::DrawText(text, 10, 200);

            std::snprintf(text, textSize, "Velocity Y: %.f M/S", body->GetVelocity().y);
            Renderer::DrawText(text, 10, 220);

            std::snprintf(text, textSize, "Velocity Z: %.f M/S", body->GetVelocity().z);
            Renderer::DrawText(text, 10, 240);

            std::snprintf(text, textSize, "Mass: %e KG", body->GetMass());
            Renderer::DrawText(text, 10, 280);

            const double dist = body->GetPosition().Distance({ 0, 0, 0 });
            if (dist != 0.0)
            {
                const double angle = std::asin(body->GetPosition().y / dist);
                std::snprintf(text, textSize, "Inclination: %.4f Radians %.2f Degrees", angle, angle * (180 / Physics::Const::Pi));
                Renderer::DrawText(text, 10, 300);
            }
        }
    }


    static void RenderStats(double elapsedTime, bool showInfoText, double simulationTime, int screenWidth) noexcept
    {
        const double daysPassed = elapsedTime / (60.0 * 60.0 * 24.0);  // seconds to days

        char text[64];
        const std::size_t textSize = ARRAY_SIZE(text);

        std::snprintf(text, textSize, "%d FPS", GetFPS());
        Renderer::DrawText(text, 10, 10);

        std::snprintf(text, textSize, "Days passed: %.2f", daysPassed);
        Renderer::DrawText(text, 10, 40);

        std::snprintf(text, textSize, "Simulation time: %.4f ms", simulationTime);
        Renderer::DrawText(text, 10, 60);

        if (showInfoText)
        {
            std::strncpy(text, "Press F1 to open the settings window", textSize);
            Renderer::DrawText(text, (screenWidth - MeasureText(text, Renderer::FontSize)) / 2, 10);
        }
    }

};