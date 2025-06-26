//
// Created by steph on 6/26/2025.
//

#pragma once
#include <memory>
#include <vector>

namespace Vox
{
    class Tickable;

    class TickManager
    {
    public:
        TickManager();

        void RegisterTickable(const std::shared_ptr<Tickable>& tickable);

        void Tick(float deltaTime);

        void Play();

    private:
        void RemoveExpiredTickables();

        std::vector<std::weak_ptr<Tickable>> prePhysicsTickables;
        std::vector<std::weak_ptr<Tickable>> physicsTickables;
        std::vector<std::weak_ptr<Tickable>> gameTickables;
    };

} // Vox
