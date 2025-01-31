#include "TypeConversions.h"

glm::vec3 Vector3From(JPH::Vec3 a)
{
	return glm::vec3(a.GetX(), a.GetY(), a.GetZ());
}

JPH::Vec3 Vec3From(glm::vec3 a)
{
	return JPH::Vec3(a.x, a.y, a.z);
}

glm::quat QuatFrom(JPH::Quat a)
{
	return glm::quat(a.GetW(), a.GetX(), a.GetY(), a.GetZ());
}
