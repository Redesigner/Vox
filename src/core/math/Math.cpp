#include "Math.h"

#include <cmath>

namespace Vox
{


	int FloorMultiple(float x, unsigned int multiple)
	{
		return std::floorf(x / static_cast<float>(multiple)) * multiple;
	}
}
