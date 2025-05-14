#pragma once
#include "core/datatypes/Ref.h"

namespace Vox
{
    class Object;
    class World;

    class WorldOutline
    {
    public:
        void Draw(World* world);

        Object* GetSelectedObject();

        void SetSelectedObject(Object* object);

    private:
        void DrawObject(Object* object);

        Object* currentlySelectedObject = nullptr;
    };
}

