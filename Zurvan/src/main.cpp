#include <cstdio>
#include <vector>

#include "raylib.h"

#include "Math.h"
#include "Physics.h"

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
//const double MOON_MASS = 7.347e22; // kg
//const double EARTH_MOON_DISTANCE = 384400000; // meters
//const double MOON_SPEED = 1022; // meters per second

const double DISTANCE_SCALE = 1e9; // 1 px = 1,000,000,000 meters
#define TIME_SCALAR 10000.0
const double TIME_STEP = 60 * 60 * TIME_SCALAR;        // 1 hour per frame
using FLOAT = double;


void Draw3DGridWithAxes(int size, float spacing)
{
    // Draw grid lines along each axis
    for (int i = -size; i <= size; i++)
    {
        // XZ plane (Y=0)
        DrawLine3D({ i * spacing, 0, -size * spacing }, { i * spacing, 0, size * spacing }, DARKGRAY);
        DrawLine3D({ -size * spacing, 0, i * spacing }, { size * spacing, 0, i * spacing }, DARKGRAY);

        // XY plane (Z=0)
        DrawLine3D({ i * spacing, -size * spacing, 0 }, { i * spacing, size * spacing, 0 }, Fade(DARKGRAY, 0.3f));
        DrawLine3D({ -size * spacing, i * spacing, 0 }, { size * spacing, i * spacing, 0 }, Fade(DARKGRAY, 0.3f));

        // YZ plane (X=0)
        DrawLine3D({ 0, i * spacing, -size * spacing }, Vector3{ 0, i * spacing, size * spacing }, Fade(DARKGRAY, 0.3f));
        DrawLine3D({ 0, -size * spacing, i * spacing }, Vector3{ 0, size * spacing, i * spacing }, Fade(DARKGRAY, 0.3f));
    }

    // Draw axis lines
    DrawLine3D({ 0, 0, 0 }, { 100, 0, 0 }, RED);   // X axis
    DrawLine3D({ 0, 0, 0 }, { 0, 100, 0 }, GREEN); // Y axis
    DrawLine3D({ 0, 0, 0 }, { 0, 0, 100 }, BLUE);  // Z axis

    // Draw origin
    DrawSphere(Vector3{ 0, 0, 0 }, 1.0f, YELLOW);
}


#define LOG_SCALE_FACTOR 50.0f  // tweak as needed

Vector3 LogMetersToWorld(Vector3 meters) {
    return Vector3 {
        copysignf(log10f(fabsf(meters.x) + 1.0f) * LOG_SCALE_FACTOR, meters.x),
            copysignf(log10f(fabsf(meters.y) + 1.0f) * LOG_SCALE_FACTOR, meters.y),
            copysignf(log10f(fabsf(meters.z) + 1.0f) * LOG_SCALE_FACTOR, meters.z)
    };
}


Vector3 MetersToWorld(Vector3 meters)
{
    return Vector3{ meters.x / (float)DISTANCE_SCALE, meters.y / (float)DISTANCE_SCALE, meters.z / (float)DISTANCE_SCALE };
}


int main()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Zurvan");

    // Camera setup
    Camera3D camera = { 0 };
    camera.position = Vector3{ 500.0f, 500.0f, 300.0f };
    camera.target = Vector3{ 0.0f, 0.0f, 0.0f };
    camera.up = Vector3{ 0.0f, 1.0f, 0.0f };
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    Physics::RigidBody<FLOAT> sun;
    sun.SetPosition(0, 0, 0);
    sun.SetVelocity(0, 0, 0);
    sun.SetMass(Physics::Const::SUN_MASS);
    sun.SetColor(YELLOW);

    Physics::RigidBody<FLOAT> earth;
    earth.SetPosition(Physics::Const::EARTH_SUN_DISTANCE, 0, 0);
    earth.SetVelocity(0, 0, Physics::Const::EARTH_SPEED);
    earth.SetMass(Physics::Const::EARTH_MASS);
    earth.SetColor(BLUE);

    Physics::RigidBody<FLOAT> juptier;
    juptier.SetPosition(Physics::Const::JUPTIER_SUN_DISTANCE, 0, 0);
    juptier.SetVelocity(0, 0, Physics::Const::JUPTIER_SPEED);
    juptier.SetMass(Physics::Const::JUPITER_MASS);
    juptier.SetColor(BROWN);

    Physics::RigidBody<FLOAT> mercury;
    mercury.SetPosition(Physics::Const::MERCURY_SUN_DISTANCE, 0, 0);
    mercury.SetVelocity(0, 0, Physics::Const::MERCURY_SPEED);
    mercury.SetMass(Physics::Const::MERCURY_MASS);
    mercury.SetColor(GRAY);

    Physics::RigidBody<FLOAT> venus;
    venus.SetPosition(Physics::Const::VENUS_SUN_DISTANCE, 0, 0);
    venus.SetVelocity(0, 0, Physics::Const::VENUS_SPEED);
    venus.SetMass(Physics::Const::VENUS_MASS);
    venus.SetColor(RED);

    Physics::RigidBody<FLOAT> mars;
    mars.SetPosition(Physics::Const::MARS_SUN_DISTANCE, 0, 0);
    mars.SetVelocity(0, 0, Physics::Const::MARS_SPEED);
    mars.SetMass(Physics::Const::MARS_MASS);
    mars.SetColor(ORANGE);

    Physics::RigidBody<FLOAT> saturn;
    saturn.SetPosition(Physics::Const::SATURN_SUN_DISTANCE, 0, 0);
    saturn.SetVelocity(0, 0, Physics::Const::SATURN_SPEED);
    saturn.SetMass(Physics::Const::SATURN_MASS);
    saturn.SetColor(VIOLET);

    Physics::RigidBody<FLOAT> uranus;
    uranus.SetPosition(Physics::Const::URANUS_SUN_DISTANCE, 0, 0);
    uranus.SetVelocity(0, 0, Physics::Const::URANUS_SPEED);
    uranus.SetMass(Physics::Const::URANUS_MASS);
    uranus.SetColor(SKYBLUE);

    Physics::RigidBody<FLOAT> neptun;
    neptun.SetPosition(Physics::Const::NEPTUN_SUN_DISTANCE, 0, 0);
    neptun.SetVelocity(0, 0, Physics::Const::NEPTUN_SPEED);
    neptun.SetMass(Physics::Const::NEPTUN_MASS);
    neptun.SetColor(DARKBLUE);

    //Physics::RigidBody<FLOAT> moonA;
    //moonA.SetPosition(EARTH_SUN_DISTANCE + EARTH_MOON_DISTANCE, 0, 0);
    //moonA.SetVelocity(0, 0, MOON_SPEED);
    //moonA.SetMass(MOON_MASS);
    //moonA.SetColor(LIGHTGRAY);

    //Physics::RigidBody<FLOAT> moonB;
    //moonB.SetPosition(0, 0, -300000000);
    //moonB.SetVelocity(900, 0, 0);
    //moonB.SetMass(MOON_MASS);
    //moonB.SetColor(ORANGE);

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
    //bodies.push_back(&moonA);
    //bodies.push_back(&moonB);

    //double elapsedTime = 0;
    DisableCursor();
    while (!WindowShouldClose())
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
            bodies[i]->SetVelocity(bodies[i]->GetVelocity() + (acc * (TIME_STEP * GetFrameTime())));
            bodies[i]->SetPosition(bodies[i]->GetPosition() + (bodies[i]->GetVelocity() * (TIME_STEP * GetFrameTime())));
        }

        UpdateCamera(&camera, CAMERA_FREE);

        // Draw
        BeginDrawing();
        ClearBackground(BLACK);
        BeginMode3D(camera);
        // Draw coordinate axes
        Draw3DGridWithAxes(100, 10.0f);

        for (int i = 0; i < bodies.size(); i++) {
            Vector3 pos = MetersToWorld(bodies[i]->GetPosition().ToRaylibVector());
            //float radius = (float)(2.0 + log10(bodies[i]->GetMass()) - 21);
            DrawSphere(pos, 10, bodies[i]->GetColor());
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
        //elapsedTime += TIME_STEP * GetFrameTime();
        //double daysPassed = elapsedTime / (60.0 * 60.0 * 24.0);  // seconds to days
        //char dayText[64];
        //sprintf_s(dayText, 64, "Days passed: %.2f", daysPassed);
        //DrawText(dayText, 10, 40, 20, WHITE);


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

        EndDrawing();
    }
    TerminateWindow();
}