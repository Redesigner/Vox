﻿#include "TestActor.h"

#include "components/MeshComponent.h"
#include "core/objects/component/Component.h"
#include "core/objects/component/TestComponent.h"

namespace Vox
{
    TestActor::TestActor()
    {
        displayName = fmt::format("{}_Default", classDisplayName);

        AttachComponent(Component::Create<MeshComponent>(this, "witch"));
    }
}
