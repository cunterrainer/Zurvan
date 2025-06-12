#pragma once

#include "Math.h"

namespace Physics
{
    namespace Const
    {
        const double G = 6.67430e-11;
    };

    struct RigidBody
    {
        Math::Vector3d position;
        Math::Vector3d velocity;
        double mass;
        Color color;
    };


    Math::Vector3d ComputeAcceleration(RigidBody* self, RigidBody* other)
    {
        Math::Vector3d direction = other->position - self->position;
        double distance = direction.Length();
        if (distance < 1.0) distance = 1.0;

        const double force = (Const::G * other->mass) / (distance * distance);
        direction = direction.Normalize() * force;
        return direction;
    }
}