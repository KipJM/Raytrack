//

#ifndef RAYTRACINGWEEKEND_TEX_UV_DEBUG_H
#define RAYTRACINGWEEKEND_TEX_UV_DEBUG_H
#include "../../texture.h"

class tex_uv_debug : public texture
{
public:
	tex_uv_debug() : texture() {}

	color value(double u, double v, const point3& p) const override
	{
		return color(u,v,0);
	}

	texture_type get_type() const override {return UV;}
};

#endif //RAYTRACINGWEEKEND_TEX_UV_DEBUG_H