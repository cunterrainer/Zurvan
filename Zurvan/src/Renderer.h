#pragma once
#include <cmath>
#include <vector>

#include "raylib.h"

#include "Physics.h"

namespace Renderer
{
    namespace Globals
    {
        inline double DISTANCE_SCALE = 700000000; // 1 px = 700,000,000 meters
        inline double RADIUS_SCALE = 1e6; // 1 px = 1,000,000 meters
    }

    constexpr Vector3 MetersToWorld(Vector3 meters, float distanceScale)
    {
        return Vector3{ meters.x / distanceScale, meters.y / distanceScale, meters.z / distanceScale };
    }
}