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
        EditorViewport();

        void Draw(const std::shared_ptr<World>& world);

        [[nodiscard]] bool GetClickViewportSpace(float& xOut, float& yOut, unsigned int clickX, unsigned int clickY) const;

        [[nodiscard]] bool GetClickViewportSpace(unsigned int& xOut, unsigned int& yOut, unsigned int clickX, unsigned int clickY) const;

        [[nodiscard]] glm::vec2 GetDimensions() const;

        [[nodiscard]] ViewportBox GetViewportBox() const;

        std::function<void(std::shared_ptr<Object>)> OnObjectSelected;

    private:
        glm::vec2 viewportDimensions = glm::vec2(800.0f, 450.0f);
        ViewportBox viewportBox{ 0, 0, 800, 450 };

        bool isFocused = false;
    };

} // Vox
