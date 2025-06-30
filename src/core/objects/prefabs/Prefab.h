//
// Created by steph on 5/27/2025.
//

#pragma once
#include <memory>
#include <string>
#include <vector>

#include "../ObjectClass.h"
#include "../properties/Property.h"
#include "PropertyOverride.h"

namespace Vox
{
    struct ObjectInitializer;
    class Object;
    class ObjectClass;

    struct AdditionalObject
    {
        std::weak_ptr<ObjectClass> objectClass;
        std::string objectName;
    };

    struct PrefabContext
    {
        explicit PrefabContext(const nlohmann::json& jsonObject);
        explicit PrefabContext(const Object* object, const std::weak_ptr<ObjectClass>& baseClass);

        void CreateOverrides(const nlohmann::json& context, const std::vector<std::string>& currentPathStack);

        std::string className;
        std::weak_ptr<ObjectClass> parentClass;

        std::vector<PropertyOverride> propertyOverrides;
        std::vector<AdditionalObject> additionalObjects;

    private:
        void CreateAdditionalObjects(const nlohmann::json& context);
    };

    class Prefab : public ObjectClass
    {
    public:
        explicit Prefab(const std::shared_ptr<PrefabContext>& context);
        ~Prefab() override;

        static std::unique_ptr<Prefab> FromObject(const Object* object, std::weak_ptr<ObjectClass> baseClass = {});
        static std::unique_ptr<Prefab> FromJson(const nlohmann::json& json);
        static std::unique_ptr<Prefab> FromFile(const std::string& filename);

        void SaveChanges(const Object* object);

        void SaveToFile(const std::string& filename) const;
        [[nodiscard]] nlohmann::ordered_json Serialize() const;

        [[nodiscard]] std::shared_ptr<PrefabContext> GetContext() const;

    private:
        static std::shared_ptr<Object> Construct(const ObjectInitializer& objectInitializer, const PrefabContext* prefabContext);

        static void OverrideProperty(const std::shared_ptr<Object>& object, const PropertyOverride& propertyOverride);

        std::shared_ptr<PrefabContext> context;
    };

} // Vox
