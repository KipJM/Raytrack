#ifndef RAYTRACINGWEEKEND_MATERIAL_H
#define RAYTRACINGWEEKEND_MATERIAL_H

#include "hittable.h"
#include "rtweekend.h"

class material
{
	public:
	virtual ~material() = default;

	virtual bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const
	{
		return false;
	}
};


class lambert : public material
{
public:
	lambert(const color& albedo) : albedo(albedo) {};

	bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override
	{
		auto scatter_direction = rec.normal + rand_unit_vector();

		// prevent degenerate scatter direction
		if (scatter_direction.near_zero())
		{
			scatter_direction = rec.normal;
		}

		scattered = ray(rec.p, scatter_direction);
		attenuation = albedo;
		return true;
	}

private:
	color albedo;
};


class metal : public material
{
public:
	metal(const color& albedo) : albedo(albedo) {};

	bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override
	{
		vec3 reflected = reflect(r_in.direction(), rec.normal);
		scattered = ray(rec.p, reflected);
		attenuation = albedo;
		return true;
	}

private:
	color albedo;
};

#endif //RAYTRACINGWEEKEND_MATERIAL_H