#include "TestActor.h"

#include "core/objects/component/Component.h"
#include "game_objects/components/MeshComponent.h"

namespace Vox
{
    TestActor::TestActor()
    {
        displayName = fmt::format("{}_Default", classDisplayName);

        AttachComponent(Component::Create<MeshComponent>(this, "witch"));
    }
}
