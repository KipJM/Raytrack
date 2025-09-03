#ifndef RAYTRACINGWEEKEND_TEX_PERLIN_H
#define RAYTRACINGWEEKEND_TEX_PERLIN_H
#include "../../texture.h"
#include "../generator/perlin.h"

class tex_perlin : public texture
{
public:
	tex_perlin(double scale) : scale(scale) {}

	color value(double u, double v, const point3& p) const override
	{
		return color::one * 0.5 * (1.0 + noise.noise(scale * p));
	}

private:
	perlin noise;
	double scale;
};

#endif //RAYTRACINGWEEKEND_TEX_PERLIN_H