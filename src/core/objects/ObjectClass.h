#pragma once
#include <functional>

#include "Object.h"
#include "properties/Property.h"

namespace Vox
{
    struct ObjectInitializer;
    class Object;

    class ObjectClass
    {
    public:
        using Constructor = std::function<std::shared_ptr<Object>(const ObjectInitializer&)>;
        ObjectClass(const Constructor& constructor, const std::shared_ptr<ObjectClass>& parent);
        virtual ~ObjectClass();

        /**
         * @brief Get the constructor for a new object of this class type
         * @return Object's constructor
         */
        [[nodiscard]] Constructor GetConstructor() const;

        template<typename T>
        [[nodiscard]] std::shared_ptr<T> Construct(ObjectInitializer objectInitializer) const requires Derived<T, Object>;

        /**
         * @brief Get the parent class of this (e.g., Actor when called from Character class)
         * @return Parent class. Can be nullptr if the parent class is Object (Object does not have its own ObjectClass)
         */
        [[nodiscard]] std::shared_ptr<ObjectClass> GetParentClass() const;

        /**
         * @brief Get all properties
         * @return Vector containing all properties belonging to this ObjectClass
         */
        [[nodiscard]] const std::vector<Property>& GetProperties() const;

        /**
         * @brief Find a member property of a given PropertyType
         * @param type Property type to search for
         * @return raw_ptr to the first property of this type. Can be nullptr if this ObjectClass does
         * not have a property of this type
         */
        [[nodiscard]] Property* GetPropertyByType(PropertyType type) const;

        /**
         * @brief Find a member property of a given PropertyType
         * @param name Property name to search for
        * @return raw_ptr to the first property of this type. Can be nullptr if this ObjectClass does
         * not have a property with this name
         */
        [[nodiscard]] Property* GetPropertyByName(const std::string& name) const;

        /**
         * @brief Get the ObjectClass's name. This is the same name used by ObjectService
         * @return ObjectClass's name
         */
        [[nodiscard]] const std::string& GetName() const;

        /**
         * @brief Get the default version of this object. Useful for seeing how properties have changed
         * @return raw_ptr to the default object. Has the same lifetime as this ObjectClass
         */
        [[nodiscard]] const Object* GetDefaultObject() const;

        /**
         * @brief Check if this ObjectClass constructs an object that derives from type
         * @tparam T Object type to compare to. Requires that this is a child of Object
         * @return true if this ObjectClass constructs a child of T, otherwise false
         */
        template <typename T>
        [[nodiscard]] bool IsA() const requires Derived<T, Object>
        {
            const ObjectClass* matchingClass = T::Class().get();
            for (const ObjectClass* objectClass = this; objectClass != nullptr; objectClass = objectClass->GetParentClass().get())
            {
                if (objectClass == matchingClass)
                {
                    return true;
                }
            }
            return false;
        }

#ifdef EDITOR
        [[nodiscard]] bool CanBeRenamed() const;
#endif

    protected:
        Constructor constructor;
        std::shared_ptr<ObjectClass> parentClass;
        std::vector<Property> properties;
        bool canBeRenamed = false;
        std::string name;

        // I would prefer to be a unique_ptr here, but because ObjectClass constructors almost always want shared ownership,
        // this works *okay*
        /**
         * @brief The default object. Automatically constructed when ObjectClass is constructed
         */
        std::shared_ptr<Object> defaultObject;
    };

    template <typename T>
    std::shared_ptr<T> ObjectClass::Construct(const ObjectInitializer objectInitializer) const requires Derived<T, Object>
    {
        assert(IsA<T>());
        return std::dynamic_pointer_cast<T>(GetConstructor()(objectInitializer));
    }


    template <>
    [[nodiscard]] bool ObjectClass::IsA<Object>() const;
}
