#ifndef RAYTRACINGWEEKEND_EMISSIVE_H
#define RAYTRACINGWEEKEND_EMISSIVE_H
#include "../../texture.h"
#include "../textures/tex_color.h"
#include "../../material.h"

class mat_emissive : public material
{
public:
	mat_emissive(shared_ptr<texture> emission) : tex(emission) {}
	mat_emissive(const color& emission) : tint(emission) {}
	mat_emissive(shared_ptr<texture> emission, const color& tint) : tex(emission), tint(tint) {}

	color emitted(double u, double v, const point3& p) const override
	{
		if (tex == nullptr)
		{
			return tint;
		} else
		{
			return tex -> value(u, v, p) * tint;
		}
	}


private:
	shared_ptr<texture> tex;
	color tint = color::one;
};

#endif //RAYTRACINGWEEKEND_EMISSIVE_H