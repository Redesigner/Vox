#include "EditorService.h"

namespace Vox
{
    EditorService::EditorService()
    {
        editor = std::make_unique<Editor>();
    }

    Editor* EditorService::GetEditor() const
    {
        return editor.get();
    }
}
