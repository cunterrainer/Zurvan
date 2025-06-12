#include "raylib.h"
#include <math.h>
#include <stdio.h>
#include "raymath.h"

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

const double G = 6.67430e-11;
const double EARTH_MASS = 5.972e24;
const double MOON_MASS = 7.347e22;
const double EARTH_MOON_DISTANCE = 384400000;
const double MOON_SPEED = 1022;
const double DISTANCE_SCALE = 1e6;       // 1 px = 1,000,000 meters
#define TIME_SCALAR 20.0
const double TIME_STEP = 60 * 60 * TIME_SCALAR;        // 1 hour per frame

typedef struct Body {
    Vector3 position;   // in meters
    Vector3 velocity;   // in m/s
    double mass;
    Color color;
} Body;

Vector3 ComputeAcceleration(Body* self, Body* other) {
    Vector3 direction = Vector3Subtract(other->position, self->position);
    double distance = Vector3Length(direction);
    if (distance < 1.0) distance = 1.0;

    double force = (G * other->mass) / (distance * distance);
    direction = Vector3Scale(Vector3Normalize(direction), (float)force);
    return direction;
}

Vector3 MetersToWorld(Vector3 meters) {
    return (Vector3) { meters.x / DISTANCE_SCALE, meters.y / DISTANCE_SCALE, meters.z / DISTANCE_SCALE };
}

Vector3 ComputeBarycenter(Body* bodies[], int count) {
    double totalMass = 0.0;
    Vector3 weighted = { 0 };

    for (int i = 0; i < count; i++) {
        totalMass += bodies[i]->mass;
        weighted = Vector3Add(weighted, Vector3Scale(bodies[i]->position, (float)bodies[i]->mass));
    }

    return Vector3Scale(weighted, 1.0f / (float)totalMass);
}


void Draw3DGridWithAxes(int size, float spacing)
{
    // Draw grid lines along each axis
    for (int i = -size; i <= size; i++)
    {
        // XZ plane (Y=0)
        DrawLine3D((Vector3) { i* spacing, 0, -size * spacing }, (Vector3) { i* spacing, 0, size* spacing }, DARKGRAY);
        DrawLine3D((Vector3) { -size * spacing, 0, i* spacing }, (Vector3) { size* spacing, 0, i* spacing }, DARKGRAY);

        // XY plane (Z=0)
        DrawLine3D((Vector3) { i* spacing, -size * spacing, 0 }, (Vector3) { i* spacing, size* spacing, 0 }, Fade(DARKGRAY, 0.3f));
        DrawLine3D((Vector3) { -size * spacing, i* spacing, 0 }, (Vector3) { size* spacing, i* spacing, 0 }, Fade(DARKGRAY, 0.3f));

        // YZ plane (X=0)
        DrawLine3D((Vector3) { 0, i* spacing, -size * spacing }, (Vector3) { 0, i* spacing, size* spacing }, Fade(DARKGRAY, 0.3f));
        DrawLine3D((Vector3) { 0, -size * spacing, i* spacing }, (Vector3) { 0, size* spacing, i* spacing }, Fade(DARKGRAY, 0.3f));
    }

    // Draw axis lines
    DrawLine3D((Vector3) { 0, 0, 0 }, (Vector3) { 100, 0, 0 }, RED);   // X axis
    DrawLine3D((Vector3) { 0, 0, 0 }, (Vector3) { 0, 100, 0 }, GREEN); // Y axis
    DrawLine3D((Vector3) { 0, 0, 0 }, (Vector3) { 0, 0, 100 }, BLUE);  // Z axis

    // Draw origin
    DrawSphere((Vector3) { 0, 0, 0 }, 1.0f, YELLOW);
}

int main()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Earth + 2 Moons - 3D Free Camera");

    // Camera setup
    Camera3D camera = { 0 };
    camera.position = (Vector3){ 500.0f, 500.0f, 300.0f };
    camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    // Bodies
    Body earth = {
        .position = { 0, 0, 0 },
        .velocity = { 0 },
        .mass = EARTH_MASS,
        .color = BLUE
    };

    Body moonA = {
        .position = { EARTH_MOON_DISTANCE, 0, 0 },
        .velocity = { 0, 0, MOON_SPEED },
        .mass = MOON_MASS,
        .color = LIGHTGRAY
    };

    Body moonB = {
        .position = { 0, 0, -300000000 },
        .velocity = { 900, 0, 0 },
        .mass = MOON_MASS,
        .color = ORANGE
    };

    double elapsedTime = 0;
    DisableCursor();
    while (!WindowShouldClose())
    {
        elapsedTime += TIME_STEP * GetFrameTime();
        // Compute gravitational accelerations
        Body* bodies[] = { &earth, &moonA, &moonB };
        Vector3 accelerations[3] = { 0 };
        
        for (int i = 0; i < 3; i++) {
            Vector3 acc = { 0 };
            for (int j = 0; j < 3; j++) {
                if (i != j) {
                    Vector3 a = ComputeAcceleration(bodies[i], bodies[j]);
                    acc = Vector3Add(acc, a);
                }
            }
            accelerations[i] = acc;
        }
        
        // Apply motion
        for (int i = 0; i < 3; i++) {
            bodies[i]->velocity = Vector3Add(bodies[i]->velocity, Vector3Scale(accelerations[i], TIME_STEP * GetFrameTime()));
            bodies[i]->position = Vector3Add(bodies[i]->position, Vector3Scale(bodies[i]->velocity, TIME_STEP * GetFrameTime()));
        }

        UpdateCamera(&camera, CAMERA_FREE);

        // Draw
        BeginDrawing();
        ClearBackground(BLACK);
        BeginMode3D(camera);
        // Draw coordinate axes
        Draw3DGridWithAxes(100, 10.0f);


        for (int i = 0; i < 3; i++) {
            Vector3 pos = MetersToWorld(bodies[i]->position);
            //float radius = (float)(2.0 + log10(bodies[i]->mass) - 21);
            DrawSphere(pos, 10, bodies[i]->color);
        }

        DrawLine3D(MetersToWorld(earth.position), MetersToWorld(moonA.position), RED);
        DrawLine3D(MetersToWorld(earth.position), MetersToWorld(moonB.position), RED);

        // Optional: show barycenter
        Vector3 bary = MetersToWorld(ComputeBarycenter(bodies, 3));
        DrawSphere(bary, 10.0f, RED);
        EndMode3D();

        // Example positions from world space
        Vector2 screenX = GetWorldToScreen((Vector3) { 105, 0, 0 }, camera);
        Vector2 screenY = GetWorldToScreen((Vector3) { 0, 105, 0 }, camera);
        Vector2 screenZ = GetWorldToScreen((Vector3) { 0, 0, 105 }, camera);

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
        double daysPassed = elapsedTime / (60.0 * 60.0 * 24.0);  // seconds to days
        char dayText[64];
        sprintf_s(dayText, 64, "Days passed: %.2f", daysPassed);
        DrawText(dayText, 10, 40, 20, WHITE);


        // Compute distances (in meters)
        double distA = Vector3Distance(earth.position, moonA.position);
        double distB = Vector3Distance(earth.position, moonB.position);

        // Compute midpoints between Earth and each moon
        Vector3 midA = Vector3Scale(Vector3Add(earth.position, moonA.position), 0.5f);
        Vector3 midB = Vector3Scale(Vector3Add(earth.position, moonB.position), 0.5f);

        // Convert midpoints to screen space
        Vector2 screenMidA = GetWorldToScreen(MetersToWorld(midA), camera);
        Vector2 screenMidB = GetWorldToScreen(MetersToWorld(midB), camera);

        // Format distance text
        char distTextA[64];
        char distTextB[64];
        sprintf_s(distTextA, 64, "%.0f km", distA / 1000.0);
        sprintf_s(distTextB, 64, "%.0f km", distB / 1000.0);

        // Measure text width for centering
        int textWidthA = MeasureText(distTextA, fontSize);
        int textWidthB = MeasureText(distTextB, fontSize);

        // Draw the distance text at the midpoints
        DrawText(distTextA, screenMidA.x - textWidthA / 2, screenMidA.y - fontSize / 2, fontSize, WHITE);
        DrawText(distTextB, screenMidB.x - textWidthB / 2, screenMidB.y - fontSize / 2, fontSize, WHITE);

        EndDrawing();
    }

    TerminateWindow();
}