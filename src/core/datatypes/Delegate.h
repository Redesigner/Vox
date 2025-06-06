//
// Created by steph on 5/23/2025.
//

#pragma once
#include <cassert>
#include <functional>
#include <ranges>
#include <unordered_map>

#include "DelegateHandle.h"

namespace Vox
{

    template <typename...Args>
    class Delegate
    {
    public:
        Delegate<Args...>()
        {
            counter = 0;
        }

        ~Delegate()
        {

        }

        DelegateHandle<Args...> RegisterCallback(std::function<void(Args...)> function)
        {
            unsigned int id = counter;
            keyMap.emplace(counter++, function);
            return DelegateHandle(this, id);
        }

        void UnregisterCallback(DelegateHandle<Args...> handle)
        {
            // assume the delegate handle was never initialized
            if (!handle.owner)
            {
                return;
            }

            assert(handle.owner == this);

            if (auto result = keyMap.find(handle.id); result != keyMap.end())
            {
                keyMap.erase(result);
            }
        }

        void operator()(Args... data)
        {
            for (const std::function<void(Args...)>& callback : keyMap | std::views::values)
            {
                callback(data...);
            }
        }

    private:
        std::unordered_map<unsigned int, std::function<void(Args...)>> keyMap;
        unsigned int counter;
    };

} // Vox
