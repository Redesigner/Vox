//
// Created by steph on 5/13/2025.
//

#pragma once

namespace Vox
{

    class Tickable
    {
    public:
        enum class TickOrder
        {
            PrePhysics,
            Physics,
            Game
        };

        virtual ~Tickable() = default;

        virtual void Tick(float DeltaTime) = 0;

        virtual TickOrder GetTickOrder() { return TickOrder::Game; }

        virtual void Play() {}
    };

} // Vox
