//
// Created by steph on 6/14/2025.
//

#pragma once

#include <fmt/format.h>
#include <glm/vec2.hpp>
#include <Jolt/Jolt.h>
#include <Jolt/Math/Vec3.h>

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
        return format_to(ctx.out(), "({}, {})", value.x, value.y);
    }
};

template<>
struct fmt::formatter<JPH::Vec3>
{
    constexpr auto parse(format_parse_context& ctx)
    {
        return ctx.begin();
    }

    template <typename Context>
    auto format(const JPH::Vec3& value, Context& ctx) const
    {
        return format_to(ctx.out(), "({}, {}, {})", value.GetX(), value.GetY(), value.GetZ());
    }
};
