/*
    This project uses raylib (zlib/libpng license).

    We have overridden the default RL_CULL_DISTANCE_FAR clipping distance (original 1000.0) to 20000.0
*/

#include <ctime>
#include <cstdio>
#include <vector>
#include <chrono>
#include <string>
#include <cassert>
#include <sstream>
#include <iostream>

#include "raygui.h"
#include "raylib.h"

using FLOAT = double;
#include "GUI.h"
#include "Math.h"
#include "Camera.h"
#include "Physics.h"

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
const double MOON_MASS = 7.347e22; // kg
const double EARTH_MOON_DISTANCE = 384400000; // meters
const double MOON_SPEED = 1022; // meters per second

const double DISTANCE_SCALE = 700000000; // 1 px = 700,000,000 meters
const double RADIUS_SCALE = 1e6; // 1 px = 1,000,000 meters
#define TIME_SCALAR 1000.0
const double TIME_STEP = 60 * 60 * TIME_SCALAR;        // 1 hour per frame


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
    return Vector3{ meters.x / (float)DISTANCE_SCALE, meters.y / (float)DISTANCE_SCALE, meters.z / (float)DISTANCE_SCALE };
}


double MetersToWorld(double meters)
{
    return meters / DISTANCE_SCALE;
}


int main()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Zurvan");
    SetWindowState(FLAG_WINDOW_RESIZABLE);

    GUI gui;

    // Camera setup
    Camera3D camera = { 0 };
    camera.position = Vector3{ 500.0f, 500.0f, 300.0f };
    camera.target = Vector3{ 0.0f, 0.0f, 0.0f };
    camera.up = Vector3{ 0.0f, 1.0f, 0.0f };
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    Physics::Planet sun    (                                 0.0,                            0.0, Physics::Const::SUN_MASS,     Physics::Const::SUN_RADIUS,     Physics::Const::SUN_INCLINE,     "Sun",     YELLOW);
    Physics::Planet earth  (Physics::Const::EARTH_SUN_DISTANCE,   -Physics::Const::EARTH_SPEED,   Physics::Const::EARTH_MASS,   Physics::Const::EARTH_RADIUS,   Physics::Const::EARTH_INCLINE,   "Earth",   BLUE);
    Physics::Planet juptier(Physics::Const::JUPTIER_SUN_DISTANCE, -Physics::Const::JUPTIER_SPEED, Physics::Const::JUPITER_MASS, Physics::Const::JUPITER_RADIUS, Physics::Const::JUPTIER_INCLINE, "Jupiter", BROWN);
    Physics::Planet mercury(Physics::Const::MERCURY_SUN_DISTANCE, -Physics::Const::MERCURY_SPEED, Physics::Const::MERCURY_MASS, Physics::Const::MERCURY_RADIUS, Physics::Const::MERCURY_INCLINE, "Mercury", GRAY);
    Physics::Planet venus  (Physics::Const::VENUS_SUN_DISTANCE,   -Physics::Const::VENUS_SPEED,   Physics::Const::VENUS_MASS,   Physics::Const::VENUS_RADIUS,   Physics::Const::VENUS_INCLINE,   "Venus",   RED);
    Physics::Planet mars   (Physics::Const::MARS_SUN_DISTANCE,    -Physics::Const::MARS_SPEED,    Physics::Const::MARS_MASS,    Physics::Const::MARS_RADIUS,    Physics::Const::MARS_INCLINE,    "Mars",    ORANGE);
    Physics::Planet saturn (Physics::Const::SATURN_SUN_DISTANCE,  -Physics::Const::SATURN_SPEED,  Physics::Const::SATURN_MASS,  Physics::Const::SATURN_RADIUS,  Physics::Const::SATURN_INCLINE,  "Saturn",  VIOLET);
    Physics::Planet uranus (Physics::Const::URANUS_SUN_DISTANCE,  -Physics::Const::URANUS_SPEED,  Physics::Const::URANUS_MASS,  Physics::Const::URANUS_RADIUS,  Physics::Const::URANUS_INCLINE,  "Uranus",  SKYBLUE);
    Physics::Planet neptun (Physics::Const::NEPTUN_SUN_DISTANCE,  -Physics::Const::NEPTUN_SPEED,  Physics::Const::NEPTUN_MASS,  Physics::Const::NEPTUN_RADIUS,  Physics::Const::NEPTUN_INCLINE,  "Neptun",  DARKBLUE);

    std::vector<Physics::RigidBody<FLOAT>*> bodies;
    bodies.push_back(&sun);
    bodies.push_back(&earth);
    bodies.push_back(&juptier);
    bodies.push_back(&mercury);
    bodies.push_back(&venus);
    bodies.push_back(&mars);
    bodies.push_back(&saturn);
    bodies.push_back(&uranus);
    bodies.push_back(&neptun);

    std::vector<Vector3> renderPositions(bodies.size());

    double elapsedTime = 0;
    DisableCursor();
    while (!WindowShouldClose())
    {
        auto start = std::chrono::high_resolution_clock::now();

        if (GetFrameTime() < 0.1f) // We need atleast 10 FPS to simulate properly
        {
            //Physics::VelocityVerlet(bodies, TIME_STEP, GetFrameTime());
            //Physics::RungeKutta4th(bodies, TIME_STEP, GetFrameTime());
            Physics::EulerIntegration(bodies, TIME_STEP, GetFrameTime());
        }

        auto end = std::chrono::high_resolution_clock::now();
        auto elapsed_ms = std::chrono::duration<double, std::milli>(end - start).count();

        UpdateCameraOverride(&camera, CAMERA_FREE);

        // Draw
        BeginDrawing();
        ClearBackground(BLACK);
        BeginMode3D(camera);
        // Draw coordinate axes
        Draw3DGridWithAxes(100, 30.0f);

        for (size_t i = 0; i < bodies.size(); i++) 
        {
            Vector3 pos = MetersToWorld(bodies[i]->GetPosition().ToRaylibVector()); 
            float renderedRadius = bodies[i]->GetRadius() / RADIUS_SCALE;

            // Quick and dirty fix to add the radius off the planet and the sun to it's position to
            // properly render it
            if (i != 0)
            {
                Vector3 direction = Vector3Normalize(pos);
                pos = Vector3Add(pos, Vector3Scale(direction, renderedRadius)); // move forward by its rendered radius
                pos = Vector3Add(pos, Vector3Scale(direction, sun.GetRadius() / RADIUS_SCALE));
            }
            renderPositions[i] = pos;
            DrawSphere(pos, renderedRadius, bodies[i]->GetColor());
        }

        //DrawLine3D(MetersToWorld(earth.GetPosition().ToRaylibVector()), MetersToWorld(moonA.GetPosition().ToRaylibVector()), RED);
        //DrawLine3D(MetersToWorld(earth.GetPosition().ToRaylibVector()), MetersToWorld(moonB.GetPosition().ToRaylibVector()), RED);

        // Optional: show barycenter
        //Vector3 bary = MetersToWorld(Physics::ComputeBarycenter(bodies, 3).ToRaylibVector());
        //DrawSphere(bary, 10.0f, RED);
        EndMode3D();

        // Example positions from world space
        Vector2 screenX = GetWorldToScreen(Vector3{ 105, 0, 0 }, camera);
        Vector2 screenY = GetWorldToScreen(Vector3{ 0, 105, 0 }, camera);
        Vector2 screenZ = GetWorldToScreen(Vector3{ 0, 0, 105 }, camera);

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
        DrawFPS(10, 10);


        for (size_t i = 0; i < bodies.size(); i++)
        {
            assert(bodies.size() == renderPositions.size() && "bodies.size() doesn't match renderPostions.size()");
            // Get their 3D position
            //Vector3 pos = MetersToWorld(bodies[i]->GetPosition().ToRaylibVector());
            Vector3 pos = renderPositions[i];
            Matrix cameraMatrix = GetCameraMatrix(camera);
            Vector4 cameraSpace = Vector4{ pos.x, pos.y, pos.z, 1.0f } * cameraMatrix;

            if (cameraSpace.z > 0)
                continue;

            // Transform to 2D screen coordinates
            Vector2 screenPos = GetWorldToScreen(pos, camera);

            // Draw label above the sphere
            DrawText(
                bodies[i]->GetLabel(),
                (int)screenPos.x - MeasureText(bodies[i]->GetLabel(), 20) / 2, // center horizontally
                (int)screenPos.y - 20, // move up 20px
                20,
                WHITE
            );
        }

        elapsedTime += TIME_STEP * GetFrameTime();
        double daysPassed = elapsedTime / (60.0 * 60.0 * 24.0);  // seconds to days
        char dayText[64];
        snprintf(dayText, 64, "Days passed: %.2f", daysPassed);
        DrawText(dayText, 10, 40, 20, WHITE);

        snprintf(dayText, 64, "Simulation time: %.4f ms", elapsed_ms);
        DrawText(dayText, 10, 60, 20, WHITE);
        
        //gui.Draw();

        // Draw Center
        Vector2 center = {
            GetScreenWidth() / 2.0f,
            GetScreenHeight() / 2.0f
        };
        DrawCircle(GetScreenWidth() / 2, GetScreenHeight() / 2, 1, WHITE);
        Ray ray = GetScreenToWorldRay(center, camera);

        static Physics::RigidBody<FLOAT>* stats = nullptr;
        for (size_t i = 0; i < bodies.size(); i++)
        {
            Vector3 pos = MetersToWorld(bodies[i]->GetPosition().ToRaylibVector());

            RayCollision collision = GetRayCollisionSphere(ray, pos, 50);
            if (collision.hit)
            {
                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                    stats = bodies[i];
                break; // only one even if multiple are hit due to increased hitbox
            }
            else
                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                    stats = nullptr;
        }

        if (stats != nullptr)
        {
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

            //snprintf(dayText, 64, "Inclination: %.4f Radians %.2f Degrees", stats->GetMass());
            //DrawText(dayText, 10, 260, 20, WHITE);
        }

        EndDrawing();
    }
    TerminateWindow();
    return 0;
}



// Compute distances (in meters)
//double distA = earth.GetPosition().Distance(moonA.GetPosition());
//double distB = earth.GetPosition().Distance(moonB.GetPosition());

// Compute midpoints between Earth and each moon
//Math::Vector3d midA = (earth.GetPosition() + moonA.GetPosition()) * 0.5;
//Math::Vector3d midB = (earth.GetPosition() + moonB.GetPosition()) * 0.5;

// Convert midpoints to screen space
//Vector2 screenMidA = GetWorldToScreen(MetersToWorld(midA.ToRaylibVector()), camera);
//Vector2 screenMidB = GetWorldToScreen(MetersToWorld(midB.ToRaylibVector()), camera);

// Format distance text
//char distTextA[64];
//char distTextB[64];
//sprintf_s(distTextA, 64, "%.0f km", distA / 1000.0);
//sprintf_s(distTextB, 64, "%.0f km", distB / 1000.0);

// Measure text width for centering
//int textWidthA = MeasureText(distTextA, fontSize);
//int textWidthB = MeasureText(distTextB, fontSize);

// Draw the distance text at the midpoints
//DrawText(distTextA, screenMidA.x - textWidthA / 2, screenMidA.y - fontSize / 2, fontSize, WHITE);
//DrawText(distTextB, screenMidB.x - textWidthB / 2, screenMidB.y - fontSize / 2, fontSize, WHITE);