//
// Created by steph on 6/6/2025.
//

#pragma once

namespace Vox
{
    class Object;
    class World;

    struct ObjectInitializer
    {
        /**
         * @brief Struct containing some common ptrs for constructing new Objects
         */
        ObjectInitializer();
        explicit ObjectInitializer(World* world);
        explicit ObjectInitializer(Object* parent);

        World* world = nullptr;
        Object* parent = nullptr;
    };

} // Vox
