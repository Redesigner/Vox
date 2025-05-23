//
// Created by steph on 5/22/2025.
//

#pragma once
#include <memory>

namespace Vox
{
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
        ActorEditor();
        ~ActorEditor();

        void Draw();

    private:
        std::shared_ptr<World> world;
        std::shared_ptr<EditorViewport> viewport;
        std::unique_ptr<WorldOutline> outline;
    };

} // Vox
