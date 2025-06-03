//
// Created by steph on 5/22/2025.
//

#pragma once
#include <memory>

#include "core/datatypes/DelegateHandle.h"

namespace Vox
{
    class ObjectClass;
    class Object;
    class DetailPanel;
}

namespace Vox
{
    class WorldOutline;
}

namespace Vox
{
    class EditorViewport;
    class World;

    class ActorEditor
    {
    public:
        ActorEditor(const ObjectClass* actorClass);
        ~ActorEditor();

        void Draw();

    private:
        void SavePrefab();

        std::shared_ptr<World> world;
        std::shared_ptr<EditorViewport> viewport;
        std::unique_ptr<WorldOutline> outline;

        DelegateHandle<bool> saveDelegate;
    };

} // Vox
