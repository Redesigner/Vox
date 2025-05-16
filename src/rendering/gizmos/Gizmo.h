//
// Created by steph on 5/15/2025.
//

#pragma once

#include <functional>

#include "core/datatypes/Ref.h"
#include "core/datatypes/Transform.h"
#include "rendering/PBRMaterial.h"

namespace Vox
{
    struct MeshInstance;

    class Gizmo
    {
    public:
        Gizmo();

        void Update();

    private:
        enum SelectedAxis : char
        {
            XSelected,
            YSelected,
            ZSelected,
            NoneSelected
        };

        void MouseMoved(int x, int y);

        void SetTransforms();

        void MouseReleased();

        void UpdateScreenSpaceVector(glm::vec3 axis);

        /**
         * @brief Get the vector of the click in world space
         * @return a pair of vectors, where first is the click's location in world space
         * and second is the click's direction
         */
        static std::pair<glm::vec3, glm::vec3> GetClickVector();

        Transform transform;
        Transform originalTransform;

        Ref<MeshInstance> xArrowMesh, yArrowMesh, zArrowMesh;

        glm::ivec2 clickLocation;

        glm::vec2 axisScreenSpaceVector;

        SelectedAxis selectedAxis = NoneSelected;

        PBRMaterial red, green, blue, yellow;

        std::function<void()> mouseReleaseCallback;
    };

} // Vox
