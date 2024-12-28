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
	UpdatePositionMatrix();
}

void Vox::Camera::SetPosition(float x, float y, float z)
{
	SetPosition(Vector3(x, y, z));
}

Vector3 Vox::Camera::GetPosition() const
{
	return position;
}

Matrix Vox::Camera::GetPositionMatrix() const
{
	return positionMatrix;
}

void Vox::Camera::SetRotation(Vector3 rotation)
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
	SetRotation(Vector3(pitch, yaw, roll));
}

Vector3 Vox::Camera::GetRotation() const
{
	return rotation;
}

Matrix Vox::Camera::GetRotationMatrix() const
{
	return rotationMatrix;
}

Vector3 Vox::Camera::GetForwardVector() const
{
	return Vector3Normalize(target - position);
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

Matrix Vox::Camera::GetInverseMatrix() const
{
	return MatrixInvert(viewProjectionMatrix);
}

void Vox::Camera::SetTarget(Vector3 targetPosition)
{
	useLookAt = true;
	target = targetPosition;
	rotationMatrix = MatrixLookAt({}, targetPosition - position, Vector3UnitY);
	UpdateViewMatrix();
}

void Vox::Camera::UpdatePositionMatrix()
{
	positionMatrix = MatrixTranslate(-position.x, -position.y, -position.z);
	UpdateViewMatrix();
}

void Vox::Camera::UpdateRotationMatrix()
{
	useLookAt = false;
	rotationMatrix = MatrixRotateXYZ(rotation);
	UpdateViewMatrix();
}

void Vox::Camera::UpdateViewMatrix()
{
	viewMatrix = useLookAt ? MatrixLookAt(position, target, Vector3UnitY) : positionMatrix * rotationMatrix;
	UpdateViewProjectionMatrix();
}

void Vox::Camera::UpdateProjectionMatrix()
{
	projectionMatrix = MatrixPerspective(fovY, aspectRatio, CAMERA_CULL_DISTANCE_NEAR, CAMERA_CULL_DISTANCE_FAR);
	UpdateViewProjectionMatrix();
}

void Vox::Camera::UpdateViewProjectionMatrix()
{
	viewProjectionMatrix = viewMatrix * projectionMatrix;
}
