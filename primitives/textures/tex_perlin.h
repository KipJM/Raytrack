#ifndef RAYTRACINGWEEKEND_TEX_PERLIN_H
#define RAYTRACINGWEEKEND_TEX_PERLIN_H
#include "../../texture.h"
#include "../generator/perlin.h"

class tex_perlin : public texture
{
public:
	tex_perlin() {}

	color value(double u, double v, const point3& p) const override
	{
		return color::one * noise.noise(p);
	}

private:
	perlin noise;
};

#endif //RAYTRACINGWEEKEND_TEX_PERLIN_H