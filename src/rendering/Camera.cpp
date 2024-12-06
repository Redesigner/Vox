#include "Camera.h"

#include "raymath.h"

const float CAMERA_CULL_DISTANCE_NEAR = 0.01f;
const float CAMERA_CULL_DISTANCE_FAR = 1000.0f;

Vox::Camera::Camera()
{
	UpdateViewMatrix();
	UpdateProjectionMatrix();
}

void Vox::Camera::SetPosition(Vector3 position)
{
	if (this->position == position)
	{
		return;
	}

	this->position = position;
	UpdateViewMatrix();
}

void Vox::Camera::SetPosition(float x, float y, float z)
{
}

void Vox::Camera::SetRotation(Vector3 rotation)
{
	if (this->rotation == rotation)
	{
		return;
	}

	this->rotation = rotation;
	UpdateViewMatrix();
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

void Vox::Camera::SetAspectRatio(double asepctRatio)
{
	if (this->aspectRatio == aspectRatio)
	{
		return;
	}

	this->aspectRatio = asepctRatio;
	UpdateProjectionMatrix();
}

Matrix Vox::Camera::GetViewMatrix() const
{
	return viewMatrix;
}

Matrix Vox::Camera::GetProjectionMatrix() const
{
	return projectionMatrix;
}

Matrix Vox::Camera::GetViewProjectionMatrix() const
{
	return viewProjectionMatrix;
}

void Vox::Camera::UpdateViewMatrix()
{
	viewMatrix = MatrixTranslate(position.x, position.y, position.z) * MatrixRotateZYX(rotation);
	UpdateViewProjecctionMatrix();
}

void Vox::Camera::UpdateProjectionMatrix()
{
	projectionMatrix = MatrixPerspective(fovY, aspectRatio, CAMERA_CULL_DISTANCE_NEAR, CAMERA_CULL_DISTANCE_FAR);
	UpdateViewProjecctionMatrix();
}

void Vox::Camera::UpdateViewProjecctionMatrix()
{
	viewProjectionMatrix = projectionMatrix * viewMatrix;
}
