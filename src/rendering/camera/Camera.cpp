#include "Camera.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#undef GLM_ENABLE_EXPERIMENTAL

#include <glm/gtc/matrix_inverse.hpp>

#include "core/logging/Logging.h"

constexpr double CAMERA_CULL_DISTANCE_NEAR = 0.01;
constexpr double CAMERA_CULL_DISTANCE_FAR = 1000.0;

namespace Vox
{
    Camera::Camera()
        : positionMatrix(glm::identity<glm::mat4x4>()), rotationMatrix(glm::identity<glm::mat4x4>())
    {
        UpdateRotationMatrix();
        UpdateViewMatrix();
        UpdateProjectionMatrix();
    }

    Camera::~Camera()
    = default;

    void Camera::SetPosition(const glm::vec3 position)
    {
        if (this->position == position)
        {
            return;
        }

        this->position = position;
        UpdatePositionMatrix();
    }

    void Camera::SetPosition(float x, float y, float z)
    {
        SetPosition(glm::vec3(x, y, z));
    }

    glm::vec3 Camera::GetPosition() const
    {
        return position;
    }

    glm::mat4x4 Camera::GetPositionMatrix() const
    {
        return positionMatrix;
    }

    void Camera::SetRotation(glm::vec3 rotation)
    {
        if (this->rotation == rotation)
        {
            return;
        }

        this->rotation = rotation;
        UpdateRotationMatrix();
    }

    void Camera::SetRotation(float pitch, float yaw, float roll)
    {
        SetRotation(glm::vec3(pitch, yaw, roll));
    }

    glm::vec3 Camera::GetRotation() const
    {
        return rotation;
    }

    glm::mat4x4 Camera::GetRotationMatrix() const
    {
        return rotationMatrix;
    }

    glm::vec3 Camera::GetForwardVector() const
    {
        return forward;
    }

    glm::vec3 Camera::GetUpVector() const
    {
        return up;
    }

    glm::vec3 Camera::GetRightVector() const
    {
        return right;
    }

    void Camera::SetFovY(double fovY)
    {
        if (this->fovY == fovY)
        {
            return;
        }

        this->fovY = fovY;
        UpdateProjectionMatrix();
    }

    void Camera::SetAspectRatio(double aspectRatioIn)
    {
        if (aspectRatio == aspectRatioIn)
        {
            return;
        }

        aspectRatio = aspectRatioIn;
        UpdateProjectionMatrix();
    }

    glm::mat4x4 Camera::GetViewMatrix() const
    {
        return viewMatrix;
    }

    glm::mat4x4 Camera::GetProjectionMatrix() const
    {
        return projectionMatrix;
    }

    glm::mat4x4 Camera::GetViewProjectionMatrix() const
    {
        return viewProjectionMatrix;
    }

    glm::mat4x4 Camera::GetInverseMatrix() const
    {
        return glm::inverse(viewProjectionMatrix);
    }

    void Camera::SetTarget(glm::vec3 targetPosition)
    {
        useLookAt = true;
        target = targetPosition;
        rotationMatrix = glm::lookAt({}, targetPosition - position, glm::vec3(0.0f, 1.0f, 0.0f));
        UpdateViewMatrix();
    }

    std::pair<glm::vec4, glm::vec4> Camera::GetWorldSpaceRay(const glm::vec2& screenspace) const
    {
        const glm::vec4 rayStartViewport = {screenspace.x, screenspace.y, -1.0f, 1.0f};
        const glm::vec4 rayEndViewport = {screenspace.x, screenspace.y, 1.0f, 1.0f};
        const glm::mat4x4 inverseCamera = glm::inverseTranspose(GetViewProjectionMatrix());
        glm::vec4 rayStartAffine = rayStartViewport * inverseCamera;
        glm::vec4 rayEndAffine = rayEndViewport * inverseCamera;
        rayStartAffine /= rayStartAffine.w;
        rayEndAffine /= rayEndAffine.w;;
        return {rayStartAffine, rayEndAffine};
    }

    void Camera::UpdatePositionMatrix()
    {
        positionMatrix = glm::translate(glm::identity<glm::mat4x4>(), {-position.x, -position.y, -position.z});
        UpdateViewMatrix();
    }

    void Camera::UpdateRotationMatrix()
    {
        useLookAt = false;
        rotationMatrix = glm::eulerAngleXYZ(rotation.x, rotation.y, rotation.z);
        UpdateLocalVectors();
        UpdateViewMatrix();
    }

    void Camera::UpdateViewMatrix()
    {
        viewMatrix = useLookAt ? glm::lookAt(position, target, {0.0f, 1.0f, 0.0f}) : rotationMatrix * positionMatrix;
        UpdateViewProjectionMatrix();
    }

    void Camera::UpdateProjectionMatrix()
    {
        projectionMatrix = glm::perspective(fovY, aspectRatio, CAMERA_CULL_DISTANCE_NEAR, CAMERA_CULL_DISTANCE_FAR);
        UpdateViewProjectionMatrix();
    }

    void Camera::UpdateViewProjectionMatrix()
    {
        viewProjectionMatrix = projectionMatrix * viewMatrix;
    }

    void Camera::UpdateLocalVectors()
    {
        const glm::mat4x4 reverseRotationMatrix = glm::eulerAngleZYX(-rotation.z, -rotation.y, -rotation.x);
        forward = reverseRotationMatrix * glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
        up = reverseRotationMatrix * glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
        right = reverseRotationMatrix * glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
    }
}