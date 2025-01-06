#include "Math.h"

#include <cmath>

namespace Vox
{
	float RemapRange(float x, float minIn, float maxIn, float minOut, float maxOut)
	{
		float inWidth = maxIn - minIn;
		float outWidth = maxOut - minOut;

		float scale = outWidth / inWidth;

		return (x - minIn) * scale + minOut;
	}

	int FloorMultiple(float x, unsigned int multiple)
	{
		return std::floorf(x / static_cast<float>(multiple)) * multiple;
	}
}
