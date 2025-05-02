#include "TestActor.h"

#include "core/objects/component/Component.h"
#include "core/objects/component/TestComponent.h"

namespace Vox
{
    TestActor::TestActor()
    {
        displayName = fmt::format("{}_Default", classDisplayName);

        RegisterComponent(Component::Create<TestComponent>(this));
    }
}
