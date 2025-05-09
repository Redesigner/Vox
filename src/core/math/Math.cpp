#include "Math.h"

#include <cmath>

namespace Vox
{


	int FloorMultiple(float x, unsigned int multiple)
    {
        return std::floorf(x / static_cast<float>(multiple)) * multiple;
    }
    unsigned int NextPowerOfTwo(unsigned int value)
    {
        value |= value >> 1;
        value |= value >> 2;
        value |= value >> 3;
        value |= value >> 4;
        value |= value >> 5;
        value |= value >> 6;
        value |= value >> 7;
        value |= value >> 8;
        value |= value >> 9;
        return ++value;
    }
} // namespace Vox
