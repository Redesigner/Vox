#include "Math.h"

#include <cmath>
#include <glm/detail/func_geometric.inl>

namespace Vox
{


	int FloorMultiple(float x, unsigned int multiple)
    {
        return std::floorf(x / static_cast<float>(multiple)) * multiple;
    }

    bool IsPowerOfTwo(const unsigned int number)
    {
        return (number & (number - 1)) == 0;
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

    // this formula comes courtesy of https://underdisc.net/blog/6_gizmos/index.html
    glm::vec3 ClosestPoint(const glm::vec3& originA, const glm::vec3& directionA,
        const glm::vec3& originB, const glm::vec3& directionB)
    {
	    const glm::vec3 delta = originB - originA;
	    const float directionDot = glm::dot(directionA, directionB);
	    if (directionDot == 1.0f)
	    {
	        return {0.0f, 0.0f, 0.0f};
	    }
	    const float dADotDelta = glm::dot(directionA, delta);
	    const float dBDotDelta = glm::dot(directionB, delta);
	    const float distance = -(-dADotDelta + directionDot * dBDotDelta) / (1.0f - directionDot * directionDot);
        return originA + directionA * distance;
    }
} // namespace Vox
