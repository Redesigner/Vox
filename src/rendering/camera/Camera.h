#pragma once

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <utility>

namespace Vox
{
	class Camera
	{
	public:
		Camera();
	    ~Camera();

		void SetPosition(glm::vec3 position);
		void SetPosition(float x, float y, float z);
		[[nodiscard]] glm::vec3 GetPosition() const;
		[[nodiscard]] glm::mat4x4 GetPositionMatrix() const;

		void SetRotation(glm::vec3 rotation);
		void SetRotation(float pitch, float yaw, float roll);
		[[nodiscard]] glm::vec3 GetRotation() const;
		[[nodiscard]] glm::mat4x4 GetRotationMatrix() const;

		[[nodiscard]] glm::vec3 GetForwardVector() const;
		[[nodiscard]] glm::vec3 GetUpVector() const;
		[[nodiscard]] glm::vec3 GetRightVector() const;

		void SetFovY(double fovY);
		void SetAspectRatio(double aspectRatioIn);

		[[nodiscard]] glm::mat4x4 GetViewMatrix() const;
		[[nodiscard]] glm::mat4x4 GetProjectionMatrix() const;
		[[nodiscard]] glm::mat4x4 GetViewProjectionMatrix() const;
		[[nodiscard]] glm::mat4x4 GetInverseMatrix() const;

		void SetTarget(glm::vec3 targetPosition);

	    std::pair<glm::vec4, glm::vec4> GetWorldSpaceRay(const glm::vec2& screenspace) const;

	private:
		void UpdatePositionMatrix();
		void UpdateRotationMatrix();
		void UpdateViewMatrix();
		void UpdateProjectionMatrix();
		void UpdateViewProjectionMatrix();
	    void UpdateLocalVectors();

		glm::vec3 position{};
		glm::vec3 rotation{};
		glm::vec3 target{};

	    glm::vec3 forward{}, right{}, up{};

		bool useLookAt = false;

		double fovY = 60.0;
		double aspectRatio = 16.0 / 9.0;

		glm::mat4x4 positionMatrix{};
		glm::mat4x4 rotationMatrix{};
		glm::mat4x4 viewMatrix{};
		glm::mat4x4 projectionMatrix{};
		glm::mat4x4 viewProjectionMatrix{};
	};
}
