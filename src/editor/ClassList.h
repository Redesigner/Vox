//
// Created by steph on 5/17/2025.
//

#pragma once
#include <functional>
#include <memory>

namespace Vox
{
    class ObjectClass;

    class ClassList
    {
        using ObjectCallback = std::function<void(const ObjectClass&)>;

    public:
        ClassList();
        ~ClassList();

        void Draw();

        void SetDoubleClickCallback(ObjectCallback callback);

    private:
        ObjectCallback doubleClickCallback;
    };
} // Vox
