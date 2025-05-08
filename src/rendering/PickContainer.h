#pragma once

#ifdef EDITOR  

#include <functional>
#include <glm/vec2.hpp>


namespace Vox
{
    class PickBuffer;
}

namespace Vox
{
    struct MeshInstance;
    
    class PickContainer
    {
        using Callback = std::function<void(glm::ivec2)>;

    public:
        PickContainer();

        unsigned int RegisterCallback(Callback callback);

        void UnregisterCallback(unsigned int key);

    private:
        std::unordered_map<unsigned int, Callback> callbacks;
        
        unsigned int counter;
    };
}

#endif