#ifndef RAYTRACINGWEEKEND_METALLIC_H
#define RAYTRACINGWEEKEND_METALLIC_H

#include "../../texture.h"
#include "../textures/tex_color.h"
#include "../../material.h"

class metallic : public material
{
public:
	metallic(const color& albedo, double roughness) : albedo(albedo), roughness(roughness < 1? roughness : 1) {}; // weird cap on roughness :|

	bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override
	{
		vec3 reflected = reflect(r_in.direction(), rec.normal);
		reflected = unit_vector(reflected) + (roughness * rand_unit_vector());
		scattered = ray(rec.p, reflected, r_in.time());
		attenuation = albedo;
		return (dot(scattered.direction(), rec.normal) > 0); // absorb if rough scatter makes ray go into object
	}

private:
	color albedo;
	double roughness;
};

#endif //RAYTRACINGWEEKEND_METALLIC_H