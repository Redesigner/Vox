//
// Created by steph on 5/22/2025.
//

#pragma once
#include <memory>

#include "core/datatypes/DelegateHandle.h"

namespace Vox
{
    class ClassList;
    class DetailPanel;
    class EditorViewport;
    class Object;
    class Prefab;
    class World;
    class WorldOutline;

    class ActorEditor
    {
    public:
        explicit ActorEditor(Prefab* actorClass);
        ~ActorEditor();

        void Draw();

    private:
        void SavePrefab();

        std::shared_ptr<World> world;
        std::shared_ptr<EditorViewport> viewport;
        std::shared_ptr<Object> rootObject;
        std::unique_ptr<WorldOutline> outline;
        std::unique_ptr<ClassList> componentList;

        DelegateHandle<bool> saveDelegate;
        DelegateHandle<bool> deleteDelegate;

        Prefab* currentPrefab;
    };

} // Vox
