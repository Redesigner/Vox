#include "Camera.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#undef GLM_ENABLE_EXPERIMENTAL

#include "core/logging/Logging.h"

constexpr double CAMERA_CULL_DISTANCE_NEAR = 0.01;
constexpr double CAMERA_CULL_DISTANCE_FAR = 1000.0;

Vox::Camera::Camera()
    :position({0.0f}), rotation({0.0f}), fovY(45.0f)
{
    positionMatrix = {};
    viewMatrix = {};
    projectionMatrix = {};

	UpdateViewMatrix();
	UpdateProjectionMatrix();
}

void Vox::Camera::SetPosition(const glm::vec3 position)
{
	if (this->position == position)
	{
		return;
	}

	this->position = position;
	UpdatePositionMatrix();
}

void Vox::Camera::SetPosition(float x, float y, float z)
{
	SetPosition(glm::vec3(x, y, z));
}

glm::vec3 Vox::Camera::GetPosition() const
{
	return position;
}

glm::mat4x4 Vox::Camera::GetPositionMatrix() const
{
	return positionMatrix;
}

void Vox::Camera::SetRotation(glm::vec3 rotation)
{
	if (this->rotation == rotation)
	{
		return;
	}

	this->rotation = rotation;
	UpdateRotationMatrix();
}

void Vox::Camera::SetRotation(float pitch, float yaw, float roll)
{
	SetRotation(glm::vec3(pitch, yaw, roll));
}

glm::vec3 Vox::Camera::GetRotation() const
{
	return rotation;
}

glm::mat4x4 Vox::Camera::GetRotationMatrix() const
{
	return rotationMatrix;
}

glm::vec3 Vox::Camera::GetForwardVector() const
{
	return glm::normalize(target - position);
}

void Vox::Camera::SetFovY(double fovY)
{
	if (this->fovY == fovY)
	{
		return;
	}

	this->fovY = fovY;
	UpdateProjectionMatrix();
}

void Vox::Camera::SetAspectRatio(double aspectRatioIn)
{
	if (aspectRatio == aspectRatioIn)
	{
		return;
	}

	aspectRatio = aspectRatioIn;
	UpdateProjectionMatrix();
}

glm::mat4x4 Vox::Camera::GetViewMatrix() const
{
	return viewMatrix;
}

glm::mat4x4 Vox::Camera::GetProjectionMatrix() const
{
	return projectionMatrix;
}

glm::mat4x4 Vox::Camera::GetViewProjectionMatrix() const
{
	return viewProjectionMatrix;
}

glm::mat4x4 Vox::Camera::GetInverseMatrix() const
{
	return glm::inverse(viewProjectionMatrix);
}

void Vox::Camera::SetTarget(glm::vec3 targetPosition)
{
	useLookAt = true;
	target = targetPosition;
	rotationMatrix = glm::lookAt({}, targetPosition - position, glm::vec3(0.0f, 1.0f, 0.0f));
	UpdateViewMatrix();
}

void Vox::Camera::UpdatePositionMatrix()
{
	positionMatrix = glm::translate(glm::identity<glm::mat4x4>(), { -position.x, -position.y, -position.z });
	UpdateViewMatrix();
}

void Vox::Camera::UpdateRotationMatrix()
{
	useLookAt = false;
	rotationMatrix = glm::eulerAngleXYZ(rotation.x, rotation.y, rotation.z);
	UpdateViewMatrix();
}

void Vox::Camera::UpdateViewMatrix()
{
	viewMatrix = useLookAt ?
        glm::lookAt(position, target, {0.0f, 1.0f, 0.0f}) :
        rotationMatrix * positionMatrix;
	UpdateViewProjectionMatrix();
}

void Vox::Camera::UpdateProjectionMatrix()
{
	projectionMatrix = glm::perspective(fovY, aspectRatio, CAMERA_CULL_DISTANCE_NEAR, CAMERA_CULL_DISTANCE_FAR);
	UpdateViewProjectionMatrix();
}

void Vox::Camera::UpdateViewProjectionMatrix()
{
	viewProjectionMatrix = projectionMatrix * viewMatrix;
}
