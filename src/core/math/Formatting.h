//
// Created by steph on 6/14/2025.
//

#pragma once

#include <fmt/format.h>
#include <glm/vec2.hpp>

template<>
struct fmt::formatter<glm::ivec2>
{
    constexpr auto parse(format_parse_context& ctx)
    {
        return ctx.begin();
    }

    template <typename Context>
    auto format(const glm::ivec2& value, Context& ctx) const
    {
        return format_to(ctx.out(), "test");
    }
};
