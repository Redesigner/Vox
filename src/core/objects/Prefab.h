//
// Created by steph on 5/27/2025.
//

#pragma once
#include <memory>
#include <string>

namespace Vox
{
    class ObjectClass;

    class Prefab
    {
    public:
        Prefab(const std::string& filename);

    private:
        std::shared_ptr<ObjectClass> parent;
    };

} // Vox
