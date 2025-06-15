/*
    This project uses raylib (zlib/libpng license).

    We have overridden the default RL_CULL_DISTANCE_FAR clipping distance (original 1000.0) to 20000.0
*/

#include "Renderer.h"
#include "Application.h"

const double MOON_MASS = 7.347e22; // kg
const double EARTH_MOON_DISTANCE = 384400000; // meters
const double MOON_SPEED = 1022; // meters per second

//const double DISTANCE_SCALE = 700000000; // 1 px = 700,000,000 meters
//const double RADIUS_SCALE = 1e6; // 1 px = 1,000,000 meters
//#define TIME_SCALAR 1000.0
//const double TIME_STEP = 60 * 60 * TIME_SCALAR;        // 1 hour per frame


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


int main()
{
    InitWindow(1280, 720, "Zurvan");
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    DisableCursor();

    Renderer::Init();

    Application app(GetScreenWidth(), GetScreenHeight());
    while (!WindowShouldClose())
    {
        const float dt = GetFrameTime();

        if (IsWindowResized())
        {
            app.SetScreenSize(GetScreenWidth(), GetScreenHeight());
        }

        app.Simulate(dt);
        app.OnUpdate(dt);
        app.OnRender();
    }

    Renderer::Shutdown();
    TerminateWindow();
}