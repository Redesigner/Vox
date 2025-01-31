#pragma once

#include <glm/gtc/quaternion.hpp>
#include <glm/vec3.hpp>
#include <Jolt/Jolt.h>
#include <Jolt/Math/Vec3.h>


glm::vec3 Vector3From(JPH::Vec3 a);
JPH::Vec3 Vec3From(glm::vec3 a);
glm::quat QuatFrom(JPH::Quat a);