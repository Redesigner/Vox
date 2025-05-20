//
// Created by steph on 5/19/2025.
//

#pragma once
#include <glm/vec2.hpp>

#include "core/objects/World.h"

namespace Vox
{
    class ColorDepthFramebuffer;

    struct ViewportBox
    {
        ViewportBox();
        ViewportBox(unsigned int left, unsigned int top, unsigned int right, unsigned int bottom);

        unsigned int left, top, right, bottom;
    };

    class EditorViewport
    {
    public:

        void Draw(const ColorDepthFramebuffer* texture, World* world);

        bool GetClickViewportSpace(float& xOut, float& yOut, unsigned int clickX, unsigned int clickY) const;

        bool GetClickViewportSpace(unsigned int& xOut, unsigned int& yOut, unsigned int clickX, unsigned int clickY) const;

        glm::vec2 GetDimensions() const;

        ViewportBox GetViewportBox() const;

    private:
        glm::vec2 viewportDimensions = glm::vec2(800.0f, 450.0f);
        ViewportBox viewportBox{ 0, 0, 800, 450 };
    };

} // Vox
