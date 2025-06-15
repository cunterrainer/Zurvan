#pragma once
#include <cmath>
#include <vector>

#include "raylib.h"

#include "Physics.h"

namespace Globals
{
    inline double DISTANCE_SCALE = 700000000; // 1 px = 700,000,000 meters
    inline double RADIUS_SCALE = 1e6; // 1 px = 1,000,000 meters
}


class Renderer
{
private:
    static constexpr int FontSize = 25;
    static inline Font s_RenderFont;
public:
    static void Init()
    {
        s_RenderFont = LoadFontEx("res/Fonts/Roboto/static/Roboto-Regular.ttf", FontSize, NULL, 0);
        SetTextureFilter(s_RenderFont.texture, TEXTURE_FILTER_POINT);
    }

    static void Shutdown()
    {
        UnloadFont(s_RenderFont);
    }

    static Vector3 MetersToWorld(Vector3 meters, float distanceScale)
    {
        return Vector3{ meters.x / distanceScale, meters.y / distanceScale, meters.z / distanceScale };
    }

    static void DrawText(const char* text, int x, int y, Color color = WHITE)
    {
        DrawTextEx(s_RenderFont, text, { static_cast<float>(x), static_cast<float>(y)}, FontSize, 0, color);
    }
};