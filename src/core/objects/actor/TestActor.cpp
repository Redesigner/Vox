#include "TestActor.h"

#include "core/objects/component/Component.h"
#include "core/objects/component/TestComponent.h"
#include "game_objects/components/MeshComponent.h"
#include "game_objects/components/SkeletalMeshComponent.h"

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
