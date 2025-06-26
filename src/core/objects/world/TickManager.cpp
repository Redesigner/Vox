//
// Created by steph on 6/26/2025.
//

#include "TickManager.h"

#include "core/objects/Tickable.h"

namespace Vox
{
    TickManager::TickManager()
    {
    }

    void TickManager::RegisterTickable(const std::shared_ptr<Tickable>& tickable)
    {
        switch (tickable->GetTickOrder())
        {
        case Tickable::TickOrder::PrePhysics:
            prePhysicsTickables.emplace_back(tickable);
            break;

        case Tickable::TickOrder::Physics:
            physicsTickables.emplace_back(tickable);
            break;

        case Tickable::TickOrder::Game:
            gameTickables.emplace_back(tickable);
            break;
        }
    }

    void TickManager::Tick(const float deltaTime)
    {
        RemoveExpiredTickables();

        for (const auto& tickable : prePhysicsTickables)
        {
            tickable.lock()->Tick(deltaTime);
        }

        for (const auto& tickable : physicsTickables)
        {
            tickable.lock()->Tick(deltaTime);
        }

        for (const auto& tickable : gameTickables)
        {
            tickable.lock()->Tick(deltaTime);
        }
    }

    void TickManager::Play()
    {
        for (const auto& tickable : prePhysicsTickables)
        {
            tickable.lock()->Play();
        }

        for (const auto& tickable : physicsTickables)
        {
            tickable.lock()->Play();
        }

        for (const auto& tickable : gameTickables)
        {
            tickable.lock()->Play();
        }
    }

    void TickManager::RemoveExpiredTickables()
    {
        std::erase_if(prePhysicsTickables, [](const std::weak_ptr<Tickable>& tickable) { return tickable.expired(); });
        std::erase_if(physicsTickables, [](const std::weak_ptr<Tickable>& tickable) { return tickable.expired(); });
        std::erase_if(gameTickables, [](const std::weak_ptr<Tickable>& tickable) { return tickable.expired(); });
    }
} // Vox