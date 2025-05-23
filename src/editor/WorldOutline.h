#pragma once
#include <memory>

namespace Vox
{
    class Gizmo;
    class Object;
    class World;

    class WorldOutline
    {
    public:
        WorldOutline();
        ~WorldOutline();

        void InitializeGizmos(World* world);

        void Draw(World* world);

        void UpdateGizmos();

        [[nodiscard]] std::weak_ptr<Object> GetSelectedObject() const;

        void SetSelectedObject(const std::weak_ptr<Object>& object);

    private:
        void DrawObject(const std::shared_ptr<Object>& object);

        std::weak_ptr<Object> currentlySelectedObject;

        std::unique_ptr<Gizmo> gizmo;

        World* tempWorld;
    };
}

