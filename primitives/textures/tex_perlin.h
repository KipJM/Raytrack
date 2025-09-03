#ifndef RAYTRACINGWEEKEND_TEX_PERLIN_H
#define RAYTRACINGWEEKEND_TEX_PERLIN_H
#include "../../texture.h"
#include "../generator/perlin.h"

class tex_perlin : public texture
{
public:
	tex_perlin(double scale, double turbulence = 1) : scale(scale), turbulence(turbulence) {}

	color value(double u, double v, const point3& p) const override
	{
		return color::half * (1 + std::sin(scale * p.z() + 10 * noise.turbulence(p, turbulence)));
	}

private:
	perlin noise;
	double scale;
	double turbulence;
};

#endif //RAYTRACINGWEEKEND_TEX_PERLIN_H