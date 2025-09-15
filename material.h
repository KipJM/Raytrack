#ifndef RAYTRACINGWEEKEND_MATERIAL_H
#define RAYTRACINGWEEKEND_MATERIAL_H

#include "hittable.h"
#include "misc.h"

class material
{
	public:
	std::string name;

	virtual ~material() = default;

	virtual bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const
	{
		return false;
	}

	virtual color emitted(double u, double v, const point3& p) const
	{
		return color::zero;
	}
};

#endif //RAYTRACINGWEEKEND_MATERIAL_H