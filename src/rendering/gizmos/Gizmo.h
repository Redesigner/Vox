//
// Created by steph on 5/15/2025.
//

#pragma once

#include <functional>

#include "core/datatypes/DelegateHandle.h"
#include "core/datatypes/Ref.h"
#include "core/datatypes/Transform.h"
#include "rendering/PBRMaterial.h"

namespace Vox
{
    class SceneRenderer;
    struct MeshInstance;

    class Gizmo
    {
    public:
        Gizmo(SceneRenderer* scene);
        ~Gizmo();

        void SetTransform(const Transform& transform);

        void Update();

        void SetVisible(bool visible);

        [[nodiscard]] Transform GetTransform() const;

    private:
        enum SelectedAxis : char
        {
            XSelected,
            YSelected,
            ZSelected,
            NoneSelected
        };

        void MouseMoved();

        void SetTransforms();

        void MouseReleased();

        /**
         * @brief Get the vector of the click in world space
         * @return a pair of vectors, where first is the click's location in world space
         * and second is the click's direction
         */
        std::pair<glm::vec3, glm::vec3> GetClickVector();

        Transform transform;
        Transform originalTransform;

        Ref<MeshInstance> xArrowMesh, yArrowMesh, zArrowMesh;

        SelectedAxis selectedAxis = NoneSelected;

        PBRMaterial red, green, blue, selected;

        DelegateHandle<> mouseReleaseCallback;
        DelegateHandle<int, int> mouseMotionCallback;
    };

} // Vox
