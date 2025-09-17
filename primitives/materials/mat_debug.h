#ifndef RAYTRACINGWEEKEND_BASIC_MATERIALS_H
#define RAYTRACINGWEEKEND_BASIC_MATERIALS_H

#include "mat_diffuse.h"
#include "../../texture.h"


class mat_debug_normal : public material
{
public:
	mat_debug_normal() : internal_lambert(mat_diffuse(color(1,1,1))){}

	bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override
	{
		internal_lambert.scatter(r_in, rec, attenuation, scattered);
		attenuation = color(unit_vector(rec.normal));
		return true;
	}

	material_type get_type() const override {return material_type::Debug_Normal;}

private:
	mat_diffuse internal_lambert;
};

#endif //RAYTRACINGWEEKEND_BASIC_MATERIALS_H