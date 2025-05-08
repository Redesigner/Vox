#pragma once
#include <concepts>
#include <type_traits>

namespace Vox
{
    template<class T, class U>
    concept Derived = std::is_base_of_v<U, T>;

    template <typename T>
    concept NumericType = std::integral<T> || std::floating_point<T>;
}
