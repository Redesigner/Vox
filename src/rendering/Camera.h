#pragma once

#include "raylib.h"

namespace Vox
{
	class Camera
	{
	public:
		Camera();

		void SetPosition(Vector3 position);
		void SetPosition(float x, float y, float z);
		Vector3 GetPosition() const;
		Matrix GetPositionMatrix() const;

		void SetRotation(Vector3 rotation);
		void SetRotation(float pitch, float yaw, float roll);
		Vector3 GetRotation() const;
		Matrix GetRotationMatrix() const;

		Vector3 GetForwardVector() const;

		void SetFovY(double fovY);
		void SetAspectRatio(double asepctRatio);

		Matrix GetViewMatrix() const;
		Matrix GetProjectionMatrix() const;
		Matrix GetViewProjectionMatrix() const;
		Matrix GetInverseMatrix() const;

		void SetTarget(Vector3 targetPosition);

	private:
		void UpdatePositionMatrix();
		void UpdateRotationMatrix();
		void UpdateViewMatrix();
		void UpdateProjectionMatrix();
		void UpdateViewProjectionMatrix();

		Vector3 position;
		Vector3 rotation;
		Vector3 target;

		bool useLookAt = false;

		double fovY = 60.0;
		double aspectRatio = 16.0 / 9.0;

		Matrix positionMatrix;
		Matrix rotationMatrix;
		Matrix viewMatrix;
		Matrix projectionMatrix;
		Matrix viewProjectionMatrix;
	};
}