#pragma once

namespace Vox
{
	float RemapRange(float x, float minIn, float maxIn, float minOut, float maxOut);

	int FloorMultiple(float x, unsigned int multiple);
}