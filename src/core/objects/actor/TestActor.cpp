#include "TestActor.h"

#include "core/objects/component/Component.h"
#include "core/objects/component/TestComponent.h"
#include "game_objects/components/MeshComponent.h"

namespace Vox
{
    TestActor::TestActor()
    {
        displayName = fmt::format("{}_Default", classDisplayName);

        AttachComponent<MeshComponent>("witch");
        RegisterComponent<TestComponent>();
    }
}
