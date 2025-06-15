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

    Vector3 MetersToWorld(Vector3 meters)
    {
        return Vector3{ meters.x / (float)Globals::DISTANCE_SCALE, meters.y / (float)Globals::DISTANCE_SCALE, meters.z / (float)Globals::DISTANCE_SCALE };
    }


    double MetersToWorld(double meters)
    {
        return meters / Globals::DISTANCE_SCALE;
    }
}