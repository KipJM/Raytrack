#ifndef RAYTRACINGWEEKEND_PERLIN_H
#define RAYTRACINGWEEKEND_PERLIN_H

#include "../../rtweekend.h"

class perlin
{
public:
	perlin()
	{
		for (int i = 0; i < point_count; i++)
		{
			randfloat[i] = rand_double();
		}
	}

private:
	static const int point_count = 256;
	double randfloat[point_count];
	int perm_x[point_count];
	int perm_y[point_count];
	int perm_z[point_count];
};

#endif //RAYTRACINGWEEKEND_PERLIN_H