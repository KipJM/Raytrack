#ifndef RAYTRACINGWEEKEND_SOLID_COLOR_H
#define RAYTRACINGWEEKEND_SOLID_COLOR_H
#include "../../texture.h"

class tex_color : public texture
{
	public:
	tex_color(const color& albedo) : albedo(albedo) {}

	tex_color(double red, double green, double blue) : tex_color(color(red, green, blue)) {}

	color value(double u, double v, const point3& p) const override
	{
		return albedo;
	}

private:
	color albedo;
};

#endif //RAYTRACINGWEEKEND_SOLID_COLOR_H