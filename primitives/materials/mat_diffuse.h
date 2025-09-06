#ifndef RAYTRACINGWEEKEND_LAMBERT_H
#define RAYTRACINGWEEKEND_LAMBERT_H

#include "../../texture.h"
#include "../textures/tex_color.h"
#include "../../material.h"

class mat_diffuse : public material
{
public:
	mat_diffuse(const color& albedo) : albedo(make_shared<tex_color>(albedo)) {};
	mat_diffuse(shared_ptr<texture> albedo) : albedo(albedo) {};

	bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override
	{
		auto scatter_direction = rec.normal + rand_unit_vector();

		// prevent degenerate scatter direction
		if (scatter_direction.near_zero())
		{
			scatter_direction = rec.normal;
		}

		scattered = ray(rec.p, scatter_direction, r_in.time());
		attenuation = albedo->value(rec.u, rec.v, rec.p);
		return true;
	}

private:
	shared_ptr<texture> albedo;
};

#endif //RAYTRACINGWEEKEND_LAMBERT_H