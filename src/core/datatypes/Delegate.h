//
// Created by steph on 5/23/2025.
//

#pragma once
#include <algorithm>
#include <functional>

#include "DelegateHandle.h"

namespace Vox
{

    template <typename...T>
    class Delegate
    {
    public:
        Delegate<T...>()
        {
            counter = 0;
        }

        DelegateHandle<T...> RegisterCallback(std::function<void(T...)> function)
        {
            unsigned int id = counter;
            keyMap.emplace(counter++, std::move(function));
            return DelegateHandle(this, id);
        }

        void UnregisterCallback(DelegateHandle<T...> handle)
        {
            assert(handle.owner == this);

            if (auto result = keyMap.find(handle.id); result != keyMap.end)
            {
                keyMap.erase(result);
            }
        }

        void operator()(T... data)
        {
            std::for_each(keyMap, [data](const std::pair<unsigned int, std::function<void(T...)>>& pair)
                {
                    pair.second(data);
                });
        }

    private:
        std::unordered_map<unsigned int, std::function<void(T...)>> keyMap;
        unsigned int counter;
    };

} // Vox
