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
	metal(const color& albedo, double roughness) : albedo(albedo), roughness(roughness < 1? roughness : 1) {}; // weird cap on roughness :|

	bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override
	{
		vec3 reflected = reflect(r_in.direction(), rec.normal);
		reflected = unit_vector(reflected) + (roughness * rand_unit_vector());
		scattered = ray(rec.p, reflected);
		attenuation = albedo;
		return (dot(scattered.direction(), rec.normal) > 0); // absorb if rough scatter makes ray go into object
	}

private:
	color albedo;
	double roughness;
};

#endif //RAYTRACINGWEEKEND_MATERIAL_H