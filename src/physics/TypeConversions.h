#pragma once

#include <glm/vec3.hpp>
#include <Jolt/Jolt.h>
#include <Jolt/Math/Vec3.h>


static glm::vec3 Vector3From(JPH::Vec3 a) { return glm::vec3(a.GetX(), a.GetY(), a.GetZ()); }
static JPH::Vec3 Vec3From(glm::vec3 a) { return JPH::Vec3(a.x, a.y, a.z); }