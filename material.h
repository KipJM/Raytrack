#ifndef RAYTRACINGWEEKEND_MATERIAL_H
#define RAYTRACINGWEEKEND_MATERIAL_H

#include "hittable.h"
#include "misc.h"

class material
{
	public:
	virtual ~material() = default;

	virtual bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const
	{
		return false;
	}
};

#endif //RAYTRACINGWEEKEND_MATERIAL_H