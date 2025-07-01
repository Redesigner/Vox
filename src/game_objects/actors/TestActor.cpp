#include "game_objects/actors/TestActor.h"

#include "../components/TestComponent.h"
#include "../components/scene_component/MeshComponent.h"
#include "../components/scene_component/SkeletalMeshComponent.h"

namespace Vox
{
    TestActor::TestActor(const ObjectInitializer& objectInitializer)
        :Actor(objectInitializer)
    {

        displayName = fmt::format("{}_Default", classDisplayName);

        AttachComponent<MeshComponent>("witch");
        AttachComponent<SkeletalMeshComponent>("scorpion");
        CreateChild<TestComponent>("Test component");
    }
}
