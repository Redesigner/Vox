#include "PickContainer.h"

#include "Renderer.h"
#include "buffers/PickBuffer.h"
#include "core/services/InputService.h"
#include "core/services/ServiceLocator.h"

namespace Vox
{
    PickContainer::PickContainer()
    {
        callbacks = {};
        counter = 1;

        ServiceLocator::GetInputService()->RegisterMouseClickCallback([this](int x, int y)
        {
            unsigned int key = ServiceLocator::GetRenderer()->GetPickBuffer()->GetValue(x, y);
            if (key == 0)
            {
                return;
            }
            
            if (const auto result = callbacks.find(key); result != callbacks.end())
            {
                result->second(glm::ivec2(x, y));
            }
        });
    }

    unsigned int PickContainer::RegisterCallback(Callback callback)
    {
        callbacks.emplace(counter, std::move(callback));
        return counter++;
    }

    void PickContainer::UnregisterCallback(const unsigned int key)
    {
        if (const auto result = callbacks.find(key); result != callbacks.end())
        {
            callbacks.erase(result);
        }
    }
}
