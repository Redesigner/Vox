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
        ~WorldOutline() = default;

        void InitializeGizmos();

        void Draw(const World* world);

        void UpdateGizmos() const;

        Object* GetSelectedObject() const;

        void SetSelectedObject(Object* object);

    private:
        void DrawObject(Object* object);

        Object* currentlySelectedObject = nullptr;

        std::unique_ptr<Gizmo> gizmo;
    };
}

