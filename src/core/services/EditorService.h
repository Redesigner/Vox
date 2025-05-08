#pragma once
#include <memory>

#include "editor/Editor.h"

namespace Vox
{
    /**
     * @class EditorService
     * @brief The editor service. Holds the editor and nothing else.
     */
    class EditorService
    {
    public:
        EditorService();

        Editor* GetEditor() const;

    private:
        std::unique_ptr<Editor> editor;
    };
}
