//
// Created by steph on 6/19/2025.
//

#pragma once
#include <glm/vec2.hpp>

template<> struct std::less<glm::ivec2>
{
    bool operator() (const glm::ivec2& left, const glm::ivec2& right) const
    {
        if (left.x == right.x)
        {
            return left.y < right.y;
        }

        return left.x < right.x;
    }
}; // Vox
