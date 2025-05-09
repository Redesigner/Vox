#pragma once
#include "core/datatypes/Ref.h"

namespace Vox
{
    class Object;
    class World;

    //@TODO: make this not static
    
    class WorldOutline
    {
    public:
        static void Draw(World* world);

        static Object* GetSelectedObject();

        static void SetSelectedObject(Object* object);

    private:
        static bool DrawObject(Object* object);

        inline static Object* currentlySelectedObject = nullptr;
    };
}

