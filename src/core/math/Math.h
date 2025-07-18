#pragma once

#include <glm/vec3.hpp>

#include "core/concepts/Concepts.h"

namespace Vox
{
	template<typename TypeIn, typename TypeOut>
	TypeOut RemapRange(TypeIn x, TypeIn minIn, TypeIn maxIn, TypeOut minOut, TypeOut maxOut) requires NumericType<TypeIn> && NumericType <TypeOut>
	{
		TypeIn inWidth = maxIn - minIn;
		TypeOut outWidth = maxOut - minOut;

		TypeOut scale = outWidth / inWidth;

		return (x - minIn) * scale + minOut;
	}
	
	template <typename T>
	T RemapRange(T x, T minIn, T maxIn, T minOut, T maxOut) requires NumericType<T>
	{
		return RemapRange<T, T>(x,  minIn, maxIn, minOut, maxOut);
	}

	int FloorMultiple(float x, unsigned int multiple);

    bool IsPowerOfTwo(unsigned int number);

    unsigned int NextPowerOfTwo(unsigned int value);

    glm::vec3 ClosestPoint(const glm::vec3& originA, const glm::vec3& directionA,
        const glm::vec3& originB, const glm::vec3& directionB);
}