#pragma once
#include <cmath>
#include <vector>

#include "raylib.h"

#include "Physics.h"

namespace Renderer
{
    namespace Globals
    {
        double DISTANCE_SCALE = 700000000; // 1 px = 700,000,000 meters
        double RADIUS_SCALE = 1e6; // 1 px = 1,000,000 meters
    }

    void Draw3DGridWithAxes(int size, float spacing)
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


    Vector3 MetersToWorld(Vector3 meters)
    {
        return Vector3{ meters.x / (float)Globals::DISTANCE_SCALE, meters.y / (float)Globals::DISTANCE_SCALE, meters.z / (float)Globals::DISTANCE_SCALE };
    }


    double MetersToWorld(double meters)
    {
        return meters / Globals::DISTANCE_SCALE;
    }


    // Modifies the render position of bodies
    void RenderPlanets(std::vector<Physics::RigidBody<FLOAT>>* bodies, const Physics::RigidBody<FLOAT>& sun)
    {
        std::vector<Physics::RigidBody<FLOAT>>& bodiesRef = *bodies;

        for (size_t i = 0; i < bodiesRef.size(); i++)
        {
            Vector3 pos = Renderer::MetersToWorld(bodiesRef[i].GetPosition().ToRaylibVector());
            float renderedRadius = (float)(bodiesRef[i].GetRadius() / Renderer::Globals::RADIUS_SCALE);

            // Quick and dirty fix to add the radius off the planet and the sun to it's position to
            // properly render it
            if (i != 0)
            {
                Vector3 direction = Vector3Normalize(pos);
                pos = Vector3Add(pos, Vector3Scale(direction, renderedRadius)); // move forward by its rendered radius
                pos = Vector3Add(pos, Vector3Scale(direction, sun.GetRadius() / Renderer::Globals::RADIUS_SCALE));
            }
            bodiesRef[i].SetRenderPos(pos);
            DrawSphere(pos, renderedRadius, bodiesRef[i].GetColor());
        }
    }


    void RenderPlanetStats(Physics::RigidBody<FLOAT>* stats)
    {
        if (stats != nullptr)
        {
            char dayText[64];
            snprintf(dayText, 64, "Name: %s", stats->GetLabel());
            DrawText(dayText, 10, 80, 20, WHITE);

            snprintf(dayText, 64, "Position X: %.f", stats->GetPosition().x);
            DrawText(dayText, 10, 100, 20, WHITE);

            snprintf(dayText, 64, "Position Y: %.f", stats->GetPosition().y);
            DrawText(dayText, 10, 120, 20, WHITE);

            snprintf(dayText, 64, "Position Z: %.f", stats->GetPosition().z);
            DrawText(dayText, 10, 140, 20, WHITE);

            snprintf(dayText, 64, "Velocity X: %.f M/S", stats->GetVelocity().x);
            DrawText(dayText, 10, 180, 20, WHITE);

            snprintf(dayText, 64, "Velocity Y: %.f M/S", stats->GetVelocity().y);
            DrawText(dayText, 10, 200, 20, WHITE);

            snprintf(dayText, 64, "Velocity Z: %.f M/S", stats->GetVelocity().z);
            DrawText(dayText, 10, 220, 20, WHITE);

            snprintf(dayText, 64, "Mass: %e KG", stats->GetMass());
            DrawText(dayText, 10, 240, 20, WHITE);

            const double dist = stats->GetPosition().Distance({ 0, 0, 0 });
            if (dist != 0.0)
            {
                const double angle = std::asin(stats->GetPosition().y / dist);
                snprintf(dayText, 64, "Inclination: %.4f Radians %.2f Degrees", angle, angle * (180 / Physics::Const::Pi));
                DrawText(dayText, 10, 260, 20, WHITE);
            }
        }
    }
}