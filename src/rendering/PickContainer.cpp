#ifdef EDITOR
#include "PickContainer.h"

#include "Renderer.h"
#include "SceneRenderer.h"
#include "buffers/frame_buffers/PickBuffer.h"
#include "core/services/EditorService.h"
#include "core/services/InputService.h"
#include "core/services/ServiceLocator.h"
#include "editor/EditorViewport.h"

namespace Vox
{
    PickContainer::PickContainer(SceneRenderer* owner)
        :owner(owner)
    {
        callbacks = {};
        counter = 1;

        ServiceLocator::GetInputService()->RegisterMouseClickCallback([this](int x, int y)
        {
            unsigned int viewX, viewY;
             if (!ServiceLocator::GetEditorService()->GetEditor()->GetViewport()->GetClickViewportSpace(viewX, viewY,x, y))
             {
                 return;
             }
            
            const unsigned int key = this->owner->GetPickBuffer()->GetValue(viewX, viewY);
            if (key == 0)
            {
                ServiceLocator::GetEditorService()->GetEditor()->SelectObject(std::weak_ptr<Object>());
                this->owner->ClearMeshOutlines();
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
        VoxLog(Display, Input, "Registering click callback for object {}", counter);
        callbacks.emplace(counter, std::move(callback));
        return counter++;
    }

    void PickContainer::UnregisterCallback(const unsigned int key)
    {
        if (const auto result = callbacks.find(key); result != callbacks.end())
        {
            VoxLog(Display, Input, "Unregistering click callback for object '{}'", key);
            callbacks.erase(result);
        }
    }
}
#endif