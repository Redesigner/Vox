#pragma once

#ifdef EDITOR  

#include <functional>
#include <glm/vec2.hpp>


namespace Vox
{
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
        PickContainer(SceneRenderer* owner, PickBuffer* attachedBuffer);

        unsigned int RegisterCallback(Callback callback);

        void UnregisterCallback(unsigned int key);

    private:
        std::unordered_map<unsigned int, Callback> callbacks;

        SceneRenderer* owner;

        PickBuffer* attachedBuffer;

        unsigned int counter;
    };
}

#endif