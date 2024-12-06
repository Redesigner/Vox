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

		void SetRotation(Vector3 rotation);
		void SetRotation(float pitch, float yaw, float roll);

		void SetFovY(double fovY);
		void SetAspectRatio(double asepctRatio);

		Matrix GetViewMatrix() const;
		Matrix GetProjectionMatrix() const;
		Matrix GetViewProjectionMatrix() const;

	private:
		void UpdateViewMatrix();
		void UpdateProjectionMatrix();
		void UpdateViewProjecctionMatrix();

		Vector3 position;
		Vector3 rotation;

		double fovY = 60.0;
		double aspectRatio = 16.0 / 9.0;

		Matrix viewMatrix;
		Matrix projectionMatrix;
		Matrix viewProjectionMatrix;
	};
}