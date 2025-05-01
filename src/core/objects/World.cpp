#include "World.h"

namespace Vox
{
    const std::vector<Object*>& World::GetObjects() const
    {
        return objects;
    }

    World::~World()
    {
        for (Object* object : objects)
        {
            delete object;
        }
        objects.clear();
    }
}
