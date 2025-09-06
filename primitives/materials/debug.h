#ifndef RAYTRACINGWEEKEND_BASIC_MATERIALS_H
#define RAYTRACINGWEEKEND_BASIC_MATERIALS_H

#include "diffuse.h"
#include "../../texture.h"


class debug_normal : public material
{
public:
	debug_normal() : internal_lambert(diffuse(color(1,1,1))){}

	bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override
	{
		internal_lambert.scatter(r_in, rec, attenuation, scattered);
		attenuation = color(unit_vector(rec.normal));
		return true;
	}

private:
	diffuse internal_lambert;
};

#endif //RAYTRACINGWEEKEND_BASIC_MATERIALS_H