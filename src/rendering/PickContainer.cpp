#ifdef EDITOR
#include "PickContainer.h"

#include "Renderer.h"
#include "SceneRenderer.h"
#include "buffers/frame_buffers/PickBuffer.h"
#include "core/logging/Logging.h"
#include "core/services/InputService.h"
#include "editor/EditorViewport.h"

namespace Vox
{
    PickContainer::PickContainer(SceneRenderer* owner)
        :owner(owner)
    {
        callbacks = {};
        counter = 1;

        masterCallback = ServiceLocator::GetInputService()->RegisterMouseClickCallback([this](int x, int y)
        {
            if (this->owner->viewport.expired())
            {
                return;
            }

            unsigned int viewX, viewY;
            EditorViewport* viewport = this->owner->viewport.lock().get();
            if (!viewport->GetClickViewportSpace(viewX, viewY,x, y))
            {
                return;
            }
            
            const unsigned int key = this->owner->GetPickBuffer()->GetValue(viewX, viewY);
            if (key == 0)
            {
                viewport->OnObjectSelected(nullptr);
                this->owner->ClearMeshOutlines();
                return;
            }
            
            if (const auto result = callbacks.find(key); result != callbacks.end())
            {
                result->second(glm::ivec2(x, y));
            }
        });
    }

    PickContainer::~PickContainer()
    {
        ServiceLocator::GetInputService()->UnregisterMouseClickCallback(masterCallback);
    }

    unsigned int PickContainer::RegisterCallback(Callback callback)
    {
        // VoxLog(Display, Input, "Registering click callback for object {}", counter);
        callbacks.emplace(counter, std::move(callback));
        return counter++;
    }

    void PickContainer::UnregisterCallback(const unsigned int key)
    {
        if (const auto result = callbacks.find(key); result != callbacks.end())
        {
            // VoxLog(Display, Input, "Unregistering click callback for object '{}'", key);
            callbacks.erase(result);
        }
    }
}
#endif