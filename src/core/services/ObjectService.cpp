#include "ObjectService.h"

#include "core/objects/TestObject.h"
#include "core/objects/TestObjectChild.h"
#include "../../game_objects/actors/Actor.h"
#include "../../game_objects/actors/TestActor.h"
#include "../../game_objects/components/TestComponent.h"

#include "game_objects/actors/character/Character.h"
#include "../../game_objects/components/scene_component/CameraComponent.h"
#include "../../game_objects/components/scene_component/MeshComponent.h"
#include "../../game_objects/components/scene_component/SkeletalMeshComponent.h"
#include "game_objects/components/physics/CharacterPhysicsComponent.h"

namespace Vox
{
    ObjectService::ObjectService()
    {
        LoadDefaultObjectClasses();
    }

    void ObjectService::RegisterPrefab(const std::string& filename)
    {
        classRegistry.emplace(filename, Prefab::FromFile(filename));
    }

    std::shared_ptr<ObjectClass> ObjectService::GetObjectClass(const std::string& objectClassId) const
    {
        auto iterator = classRegistry.find(objectClassId);
        if (iterator == classRegistry.end())
        {
            return nullptr;
        }
        return iterator->second;
    }

    std::map<std::string, std::shared_ptr<ObjectClass>>::const_iterator ObjectService::GetBegin() const
    {
        return classRegistry.cbegin();
    }

    std::map<std::string, std::shared_ptr<ObjectClass>>::const_iterator ObjectService::GetEnd() const
    {
        return classRegistry.cend();
    }

    DelegateHandle<const ObjectClass*> ObjectService::RegisterClassChangedDelegate(
        const std::function<void(const ObjectClass*)>& delegate)
    {
        return objectClassChanged.RegisterCallback(delegate);
    }

    void ObjectService::UnregisterClassChangedDelegate(DelegateHandle<const ObjectClass*> handle)
    {
        objectClassChanged.UnregisterCallback(handle);
    }

    void ObjectService::UpdateClass(const ObjectClass* objectClass)
    {
        objectClassChanged(objectClass);
    }

    void ObjectService::LoadDefaultObjectClasses()
    {
        RegisterObjectClass<Actor>();
        RegisterObjectClass<Component>();
        RegisterObjectClass<SceneComponent>();
        RegisterObjectClass<TestObject>();
        RegisterObjectClass<TestObjectChild>();
        RegisterObjectClass<TestActor>();
        RegisterObjectClass<Character>();
        RegisterObjectClass<TestComponent>();
        RegisterObjectClass<MeshComponent>();
        RegisterObjectClass<SkeletalMeshComponent>();
        RegisterObjectClass<CameraComponent>();
        RegisterObjectClass<CharacterPhysicsComponent>();
    }
}
