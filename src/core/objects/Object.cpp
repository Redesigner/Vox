#include "Object.h"

namespace Vox
{
    Object::Object()
    {
    }
    
    void Object::InitProperties()
    {
        BuildProperties(GetProperties());
    }
}
