//
// Created by steph on 6/6/2025.
//

#pragma once

namespace Vox
{
    class Actor;
    class Object;
    class World;

    struct ObjectInitializer
    {
        /**
         * @brief Struct containing some common ptrs for constructing new Objects
         */
        ObjectInitializer();
        explicit ObjectInitializer(World* world);
        explicit ObjectInitializer(Actor* parent);

        World* world = nullptr;
        Actor* parent = nullptr;

        /**
         * @brief Is this object's class the outermost class?
         * If true, this will call PostConstruct after construction
         */
        bool rootObject = false;
    };

} // Vox
