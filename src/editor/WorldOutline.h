#pragma once
#include <functional>
#include <memory>

#include "core/datatypes/DelegateHandle.h"

namespace Vox
{
    class Gizmo;
    class Object;
    class World;

    class WorldOutline
    {
        using ClickCallback = std::function<void(std::weak_ptr<Object>)>;

    public:
        WorldOutline();
        ~WorldOutline();

        void InitializeGizmos(World* world);

        void Draw(World* world);

        void UpdateGizmos();

        [[nodiscard]] std::weak_ptr<Object> GetSelectedObject() const;

        void SetSelectedObject(const std::weak_ptr<Object>& object);

        void SetDoubleClickCallback(ClickCallback callback);

    private:
        void DrawObject(const std::shared_ptr<Object>& object);

        std::weak_ptr<Object> currentlySelectedObject;

        std::unique_ptr<Gizmo> gizmo;

        ClickCallback doubleClickCallback;

        World* tempWorld;
    };
}

