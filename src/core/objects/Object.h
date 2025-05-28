#pragma once
#include <functional>
#include <regex>
#include <vector>
#include <fmt/format.h>
#include <nlohmann/adl_serializer.hpp>

#include "core/concepts/Concepts.h"
#include "core/objects/Property.h"

#define IMPLEMENT_NAME(Name) private: inline static std::string classDisplayName = std::regex_replace(std::string(#Name), std::regex("(\\B[A-Z])"), " $1");\
    public: std::string& GetClassDisplayName() const override { return classDisplayName; }\
    private:

#define DEFAULT_DISPLAY_NAME() displayName = fmt::format("{}_Default", classDisplayName);

/// Implement the objects name and accessors for its ObjectClass
#define IMPLEMENT_OBJECT(Name) private: static inline const ObjectClass* objectClass = nullptr;\
    public: static void SetObjectClass(const ObjectClass* objectClassIn) { objectClass = objectClassIn; }\
    const ObjectClass* GetClass() const override { assert(objectClass && "Class did not exist"); return objectClass; }\
    IMPLEMENT_NAME(Name)\
    private:

namespace Vox
{
    class Object;
    class World;

    struct ObjectInitializer
    {
        ObjectInitializer();
        explicit ObjectInitializer(World* world);
        explicit ObjectInitializer(Object* parent);

        World* world = nullptr;
        Object* parent = nullptr;
    };
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

        [[nodiscard]] const std::vector<Property>& GetProperties() const;

        [[nodiscard]] virtual const ObjectClass* GetClass() const = 0;

        [[nodiscard]] virtual std::string& GetClassDisplayName() const = 0;

        virtual void PropertyChanged(const Property& property) {}

        [[nodiscard]] const std::string& GetDisplayName() const;

        virtual void BuildProperties(std::vector<Property>& propertiesInOut) = 0;

        void SetName(const std::string& name);

        void AddChild(const std::shared_ptr<Object>& child);

        [[nodiscard]] const std::vector<std::shared_ptr<Object>>& GetChildren() const;

        [[nodiscard]] nlohmann::ordered_json Serialize();

        [[nodiscard]] virtual Object* GetParent() const;

        [[nodiscard]] std::shared_ptr<Object> GetSharedChild() const;

        [[nodiscard]] std::shared_ptr<Object> GetSharedThis() const;

        [[nodiscard]] std::shared_ptr<Object> GetSharedChild(Object* object) const;
        
    protected:
        template<typename T>
        void CreateChild(const std::string& name)
        {
            std::shared_ptr<T> newObject = T::template GetConstructor<T>()(ObjectInitializer(this));
            newObject->SetName(name);
            AddChild(newObject);
        }

        std::string displayName;

    private:
        // @TODO: rework this into weak ptrs?
        Object* parent;

        std::vector<std::shared_ptr<Object>> children;
        
    };
}
