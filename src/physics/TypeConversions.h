#pragma once

#include "Jolt/Jolt.h"
#include "Jolt/Math/Vec3.h"

#include "raylib.h"

static Vector3 Vector3From(JPH::Vec3 a) { return Vector3(a.GetX(), a.GetY(), a.GetZ()); }
static JPH::Vec3 Vec3From(Vector3 a) { return JPH::Vec3(a.x, a.y, a.z); }