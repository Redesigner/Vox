#pragma once

struct Vector3i
{
	Vector3i();
	Vector3i(int x, int y, int z);

	int x, y, z;
};

struct Vector3iu
{
	Vector3iu();
	Vector3iu(unsigned int x, unsigned int y, unsigned int z);

	unsigned int x, y, z;
};