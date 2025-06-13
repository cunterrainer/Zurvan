#pragma once
#include "raylib.h"
#include "raymath.h"
#include "rcamera.h"

namespace CameraConst
{
    constexpr float MoveSpeed = 200.f;
    constexpr float RotationSpeed = 1.5f;
    constexpr float PanSpeed = 0.2f;
    constexpr float MouseMoveSensitivity = 0.003f;
    constexpr float OrbitalSpeed = 0.5f;
};

// Override the default raylib UpdateCamera to properly adjust speeds e.g. move speed
// without modifying raylibs source code for easier upgradability
void UpdateCameraOverride(Camera* camera, int mode)
{
    Vector2 mousePositionDelta = GetMouseDelta();

    const bool moveInWorldPlane = ((mode == CAMERA_FIRST_PERSON) || (mode == CAMERA_THIRD_PERSON));
    const bool rotateAroundTarget = ((mode == CAMERA_THIRD_PERSON) || (mode == CAMERA_ORBITAL));
    const bool lockView = ((mode == CAMERA_FREE) || (mode == CAMERA_FIRST_PERSON) || (mode == CAMERA_THIRD_PERSON) || (mode == CAMERA_ORBITAL));
    const bool rotateUp = false;

    float cameraMoveSpeedAcceleration = 1.f;
    if (IsKeyDown(KEY_LEFT_CONTROL))
        cameraMoveSpeedAcceleration = 4.f;

    // Camera speeds based on frame time
    const float cameraMoveSpeed = CameraConst::MoveSpeed * GetFrameTime() * cameraMoveSpeedAcceleration;
    const float cameraRotationSpeed = CameraConst::RotationSpeed * GetFrameTime();
    const float cameraPanSpeed = CameraConst::PanSpeed * GetFrameTime();
    const float cameraOrbitalSpeed = CameraConst::OrbitalSpeed * GetFrameTime();

    if (mode == CAMERA_CUSTOM) {}
    else if (mode == CAMERA_ORBITAL)
    {
        // Orbital can just orbit
        Matrix rotation = MatrixRotate(GetCameraUp(camera), cameraOrbitalSpeed);
        Vector3 view = Vector3Subtract(camera->position, camera->target);
        view = Vector3Transform(view, rotation);
        camera->position = Vector3Add(camera->target, view);
    }
    else
    {
        // Camera rotation
        if (IsKeyDown(KEY_DOWN)) CameraPitch(camera, -cameraRotationSpeed, lockView, rotateAroundTarget, rotateUp);
        if (IsKeyDown(KEY_UP)) CameraPitch(camera, cameraRotationSpeed, lockView, rotateAroundTarget, rotateUp);
        if (IsKeyDown(KEY_RIGHT)) CameraYaw(camera, -cameraRotationSpeed, rotateAroundTarget);
        if (IsKeyDown(KEY_LEFT)) CameraYaw(camera, cameraRotationSpeed, rotateAroundTarget);
        if (IsKeyDown(KEY_Q)) CameraRoll(camera, -cameraRotationSpeed);
        if (IsKeyDown(KEY_E)) CameraRoll(camera, cameraRotationSpeed);

        // Camera movement
        // Camera pan (for CAMERA_FREE)
        if ((mode == CAMERA_FREE) && (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE)))
        {
            const Vector2 mouseDelta = GetMouseDelta();
            if (mouseDelta.x > 0.0f) CameraMoveRight(camera, cameraPanSpeed, moveInWorldPlane);
            if (mouseDelta.x < 0.0f) CameraMoveRight(camera, -cameraPanSpeed, moveInWorldPlane);
            if (mouseDelta.y > 0.0f) CameraMoveUp(camera, -cameraPanSpeed);
            if (mouseDelta.y < 0.0f) CameraMoveUp(camera, cameraPanSpeed);
        }
        else
        {
            // Mouse support
            CameraYaw(camera, -mousePositionDelta.x * CameraConst::MouseMoveSensitivity, rotateAroundTarget);
            CameraPitch(camera, -mousePositionDelta.y * CameraConst::MouseMoveSensitivity, lockView, rotateAroundTarget, rotateUp);
        }

        // Keyboard support
        if (IsKeyDown(KEY_W)) CameraMoveForward(camera, cameraMoveSpeed, moveInWorldPlane);
        if (IsKeyDown(KEY_A)) CameraMoveRight(camera, -cameraMoveSpeed, moveInWorldPlane);
        if (IsKeyDown(KEY_S)) CameraMoveForward(camera, -cameraMoveSpeed, moveInWorldPlane);
        if (IsKeyDown(KEY_D)) CameraMoveRight(camera, cameraMoveSpeed, moveInWorldPlane);

        // Gamepad movement
        if (IsGamepadAvailable(0))
        {
            // Gamepad controller support
            CameraYaw(camera, -(GetGamepadAxisMovement(0, GAMEPAD_AXIS_RIGHT_X) * 2) * CameraConst::MouseMoveSensitivity, rotateAroundTarget);
            CameraPitch(camera, -(GetGamepadAxisMovement(0, GAMEPAD_AXIS_RIGHT_Y) * 2) * CameraConst::MouseMoveSensitivity, lockView, rotateAroundTarget, rotateUp);

            if (GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_Y) <= -0.25f) CameraMoveForward(camera, cameraMoveSpeed, moveInWorldPlane);
            if (GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X) <= -0.25f) CameraMoveRight(camera, -cameraMoveSpeed, moveInWorldPlane);
            if (GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_Y) >= 0.25f) CameraMoveForward(camera, -cameraMoveSpeed, moveInWorldPlane);
            if (GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X) >= 0.25f) CameraMoveRight(camera, cameraMoveSpeed, moveInWorldPlane);
        }

        if (mode == CAMERA_FREE)
        {
            if (IsKeyDown(KEY_SPACE)) CameraMoveUp(camera, cameraMoveSpeed);
            if (IsKeyDown(KEY_LEFT_SHIFT)) CameraMoveUp(camera, -cameraMoveSpeed);
        }
    }

    if ((mode == CAMERA_THIRD_PERSON) || (mode == CAMERA_ORBITAL) || (mode == CAMERA_FREE))
    {
        // Zoom target distance
        CameraMoveToTarget(camera, -GetMouseWheelMove());
        if (IsKeyPressed(KEY_KP_SUBTRACT)) CameraMoveToTarget(camera, 2.0f);
        if (IsKeyPressed(KEY_KP_ADD)) CameraMoveToTarget(camera, -2.0f);
    }
}