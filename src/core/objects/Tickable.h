//
// Created by steph on 5/13/2025.
//

#pragma once

namespace Vox
{

    class Tickable
    {
    public:
        virtual ~Tickable() = default;

        virtual void Tick(float DeltaTime) = 0;

        virtual void Play() {}
    };

} // Vox
