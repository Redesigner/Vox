//
// Created by steph on 5/23/2025.
//

#pragma once

namespace Vox
{
    template <typename...T>
    struct DelegateHandle
    {
	    template <typename...> friend class Delegate;

        DelegateHandle()
            :owner(nullptr), id(0)
        {
        }

        DelegateHandle(Delegate<T...>* owner, const unsigned int id)
            :owner(owner), id(id)
        {
        }

    private:
        Delegate<T...>* owner;
        unsigned int id;
    };

} // Vox
