#pragma once

#ifdef EDITOR  

#include <functional>
#include <glm/vec2.hpp>
#include <memory>


namespace Vox
{
    class EditorViewport;
    class SceneRenderer;
    class PickBuffer;
}

namespace Vox
{
    struct MeshInstance;
    
    class PickContainer
    {
    using Callback = std::function<void(glm::ivec2)>;

    public:
        explicit PickContainer(SceneRenderer* owner);
        ~PickContainer();

        PickContainer(const PickContainer&) = delete;
        PickContainer(PickContainer&&) = delete;
        PickContainer& operator=(const PickContainer&) = delete;
        PickContainer& operator=(PickContainer&&) = delete;

        unsigned int RegisterCallback(Callback callback);

        void UnregisterCallback(unsigned int key);

    private:
        std::unordered_map<unsigned int, Callback> callbacks;

        SceneRenderer* owner;

        unsigned int counter;

        std::function<void(int,int)> masterCallback;
    };
}

#endif