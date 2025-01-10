#pragma once

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

namespace Vox
{
	class Camera
	{
	public:
		Camera();

		void SetPosition(glm::vec3 position);
		void SetPosition(float x, float y, float z);
		glm::vec3 GetPosition() const;
		glm::mat4x4 GetPositionMatrix() const;

		void SetRotation(glm::vec3 rotation);
		void SetRotation(float pitch, float yaw, float roll);
		glm::vec3 GetRotation() const;
		glm::mat4x4 GetRotationMatrix() const;

		glm::vec3 GetForwardVector() const;

		void SetFovY(double fovY);
		void SetAspectRatio(double asepctRatio);

		glm::mat4x4 GetViewMatrix() const;
		glm::mat4x4 GetProjectionMatrix() const;
		glm::mat4x4 GetViewProjectionMatrix() const;
		glm::mat4x4 GetInverseMatrix() const;

		void SetTarget(glm::vec3 targetPosition);

	private:
		void UpdatePositionMatrix();
		void UpdateRotationMatrix();
		void UpdateViewMatrix();
		void UpdateProjectionMatrix();
		void UpdateViewProjectionMatrix();

		glm::vec3 position;
		glm::vec3 rotation;
		glm::vec3 target;

		bool useLookAt = false;

		double fovY = 60.0;
		double aspectRatio = 16.0 / 9.0;

		glm::mat4x4 positionMatrix;
		glm::mat4x4 rotationMatrix;
		glm::mat4x4 viewMatrix;
		glm::mat4x4 projectionMatrix;
		glm::mat4x4 viewProjectionMatrix;
	};
}