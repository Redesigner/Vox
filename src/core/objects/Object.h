#pragma once
#include <functional>
#include <regex>
#include <vector>
#include <fmt/format.h>
#include <nlohmann/adl_serializer.hpp>

#include "prefabs/PropertyOverride.h"
#include "core/concepts/Concepts.h"
#include "properties/Property.h"
#include "core/objects/ObjectInitializer.h"

#define IMPLEMENT_NAME(Name) private: inline static std::string classDisplayName = std::regex_replace(std::string(#Name), std::regex("(\\B[A-Z])"), " $1");\
    public: std::string& GetClassDisplayName() const override { return classDisplayName; }\
    private:

#define DEFAULT_DISPLAY_NAME() displayName = fmt::format("{}_Default", classDisplayName);

#define IMPLEMENT_OBJECT_ROOT(Name)\
    friend class ObjectService;\
    private:\
    static inline std::shared_ptr<ObjectClass> objectClass = nullptr;\
    static void SetStaticObjectClass(const std::shared_ptr<ObjectClass>& objectClassIn) { objectClass = objectClassIn; }\
    public:\
    std::shared_ptr<ObjectClass> GetClass() const override { return localClass ? localClass : objectClass; }\
    static std::shared_ptr<ObjectClass> Class() { return objectClass; }\

/// Implement the objects name and accessors for its ObjectClass
#define IMPLEMENT_OBJECT(Name, Parent)\
    IMPLEMENT_OBJECT_ROOT(Name)\
    static std::shared_ptr<ObjectClass> GetParentClass() { return Parent::Class(); }\
    IMPLEMENT_NAME(Name)\
    private:

/// Implement an object without a defined parent class
#define IMPLEMENT_OBJECT_BASE(Name)\
    IMPLEMENT_OBJECT_ROOT(Name)\
    static std::shared_ptr<ObjectClass> GetParentClass() { return nullptr; }\
    IMPLEMENT_NAME(Name)\
    private:

namespace Vox
{
    class Object;
    class World;
    class ObjectClass;

    /// Object is an object type that has reflected properties
    class Object
    {
    public:
        explicit Object(const ObjectInitializer& objectInitializer);
        virtual ~Object() = default;

        Object(Object&) = delete;
        Object(Object&&) = delete;
        void operator =(Object&) = delete;
        Object& operator =(Object&&) = delete;
        
        template <typename T>
        static std::function<std::shared_ptr<T>(const ObjectInitializer&)> GetConstructor() requires Derived<T, Object>
        {
            return [] (const ObjectInitializer& objectInitializer){ return std::make_shared<T>(objectInitializer); };
        }

        /**
         * @brief Called when constructing the properties list for the ObjectClass definition
         * Should call the parent class's function as well
         * @param propertiesInOut Property array to be modified. REGISTER_PROPERTY automatically modifies this.
         */
        virtual void BuildProperties(std::vector<Property>& propertiesInOut) = 0;

        /**
         * @brief Called after an object is constructed, or if it belongs to a prefab, <b>after</b> the properties have been overriden.
         * Must call Super::PostConstruct, or child objects will not have PostConstruct called.
         */
        virtual void PostConstruct();

        /**
         * @brief Get the ObjectClass from ObjectService
         * @return shared ptr to the Object's class. If the object is a primitive, returns the object's local class,
         * otherwise, returns the static object class
         */
        [[nodiscard]] virtual std::shared_ptr<ObjectClass> GetClass() const = 0;

        /**
         * @brief Get the class's display name. This is called by ObjectService when constructing a new ObjectClass
         * @return Class display name
         */
        [[nodiscard]] virtual std::string& GetClassDisplayName() const = 0;

        /**
         * @brief Called whenever a property value is changed by the editor
         * @param property Property that was changed
         */
        virtual void PropertyChanged(const Property& property) {}


        /**
         * @brief Helper function for getting the properties array from the ObjectClass.
         * Fails if the object class is not set.
         * @return Properties list
         */
        [[nodiscard]] const std::vector<Property>& GetProperties() const;

        /**
         * @brief Get the Object instance's display name
         * @return Object's name, in a friendly format
         */
        [[nodiscard]] const std::string& GetDisplayName() const;

        /**
         * @brief Get all children
         * @return List of all children
         */
        [[nodiscard]] const std::vector<std::shared_ptr<Object>>& GetChildren() const;

        /**
         * @brief Add an already constructed object to this one, as a child. Implies ownership,
         * even though other objects may have weak_ptrs to the child. The child will be renamed
         * if its name matches another child.
         * @param child Child to be added
         */
        void AddChild(const std::shared_ptr<Object>& child);

        /**
         * @brief Set the display name of this object instance
         * @param name New name
         */
        void SetName(const std::string& name);

        /**
         * @brief Convert this object into a JSON object. Only saves differences from the default value.
         * @param defaultObject A default object to compare property values with.
         * @return Newly constructed JSON object
         */
        [[nodiscard]] nlohmann::ordered_json Serialize(const Object* defaultObject = nullptr);

        /**
         * @brief Generate the differences in this object's property values and the default object's property values
         * @return Vector of PropertyOverrides describing the differences
         */
        [[nodiscard]] std::vector<PropertyOverride> GenerateOverrides(const ObjectClass* defaultClass = nullptr) const;

        /**
         * @brief Get the parent object of this
         * @return Parent. Can be nullptr
         */
        [[nodiscard]] Object* GetParent() const;

        [[nodiscard]] const Object* GetRoot() const;

        [[nodiscard]] std::vector<std::string> GetRootPath() const;

        [[nodiscard]] std::shared_ptr<Object> GetChildByPath(const std::vector<std::string>& path) const;

        /**
         * @brief Get a shared_ptr to this object from the parent
         * @return shared_ptr to this. Can be nullptr if parent is nullptr
         */
        [[nodiscard]] std::shared_ptr<Object> GetSharedThis() const;

        /**
         * @brief Get a shared_ptr of a child object
         * @param object Object to get the shared_ptr of
         * @return shared_ptr. Can be nullptr if object is not a child of this
         */
        [[nodiscard]] std::shared_ptr<Object> GetSharedChild(const Object* object) const;

        /**
         * @brief Lookup a child Object by name
         * @param name Name to search for
         * @return shared_ptr. Can be nullptr if no child has this name
         */
        [[nodiscard]] std::shared_ptr<Object> GetChildByName(const std::string& name) const;

        /**
         * @brief Remove a child object
         * @param object object to compare raw_ptrs with
         * @return true if the object was removed, otherwise false
         */
        bool RemoveChild(const Object* object);

        /**
         * @brief true if this object was created from a .cpp file, false if the object was created from a prefab
         */
        bool native = true;

        /**
         * @brief Object instance's ObjectClass. nullptr for native classes, should only be set when constructing prefabs,
         * which otherwise do not have an ObjectClass
         */
        std::shared_ptr<ObjectClass> localClass;

    protected:
        /**
         * @brief Construct and add a child to this.
         * @tparam T Object type to construct. Must be derived from Object
         * @param name Child object's name
         */
        template<typename T>
        void CreateChild(const std::string& name) requires Derived<T, Object>
        {
            std::shared_ptr<T> newObject = T::template GetConstructor<T>()(ObjectInitializer(this));
            newObject->SetName(name);
            AddChild(newObject);
        }

        /**
         * @brief Method called when a child is removed. Can be used for additional
         * cleanup logic, for instance removing an attachment from an actor
         * @param object ptr to the object that was removed
         */
        virtual void ChildRemoved(const Object* object) {};

        /**
         * @brief Method called when a child is added to this.
         * @param child shared_ptr to the object that was added
         */
        virtual void ChildAdded(const std::shared_ptr<Object>& child) {};

        std::string displayName;

    private:
        [[nodiscard]] std::vector<PropertyOverride> GenerateOverrides(const Object* defaultObject, std::vector<std::string>& context) const;

        // @TODO: rework this into weak ptrs?
        Object* parent;

        std::vector<std::shared_ptr<Object>> children;
    };
}
