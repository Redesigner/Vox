//
// Created by steph on 5/17/2025.
//

#pragma once
#include <functional>
#include <memory>
#include <string>

namespace Vox
{
    class ObjectClass;

    class ClassList
    {
        using ObjectCallback = std::function<void(std::shared_ptr<ObjectClass>)>;

    public:
        ClassList();
        ~ClassList();

        void Draw();

        void SetDoubleClickCallback(ObjectCallback callback);

        void SetClassFilter(std::function<bool(const std::shared_ptr<ObjectClass>&)> filter);

        std::string title;

        std::string objectClassPayloadType;

    private:
        std::function<bool(const std::shared_ptr<ObjectClass>&)> classFilter;

        ObjectCallback doubleClickCallback;

        ObjectClass* selectedObjectClass;
    };
} // Vox
