//
// Created by steph on 6/6/2025.
//

#include "ObjectInitializer.h"

namespace Vox
{
    ObjectInitializer::ObjectInitializer()
        = default;

    ObjectInitializer::ObjectInitializer(World* world)
        :world(world)
    {
    }

    ObjectInitializer::ObjectInitializer(Object* parent)
        :parent(parent)
    {
    }
} // Vox