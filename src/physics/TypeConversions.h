#pragma once

#include "Jolt/Jolt.h"
#include "Jolt/Math/Vec3.h"

#include "raylib.h"

static Vector3 Vector3From(JPH::Vec3 a) { return Vector3(a.GetX(), a.GetY(), a.GetZ()); }