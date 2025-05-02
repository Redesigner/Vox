#include "Object.h"

namespace Vox
{    
    void Object::InitProperties()
    {
        BuildProperties(GetProperties());
    }

    const std::string& Object::GetDisplayName() const
    {
        return displayName;
    }
}
