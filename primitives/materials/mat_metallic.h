#ifndef RAYTRACINGWEEKEND_METALLIC_H
#define RAYTRACINGWEEKEND_METALLIC_H

#include "../../texture.h"
#include "../textures/tex_color.h"
#include "../../material.h"

class mat_metallic : public material
{
public:
	mat_metallic(const color& albedo, double roughness) : albedo(make_shared<tex_color>(albedo)), roughness(roughness < 1? roughness : 1) {}; // weird cap on roughness :|

	bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override
	{
		vec3 reflected = reflect(r_in.direction(), rec.normal);
		reflected = unit_vector(reflected) + (roughness * rand_unit_vector());
		scattered = ray(rec.p, reflected, r_in.time());
		attenuation = albedo->value(rec.u, rec.v, rec.p);
		return (dot(scattered.direction(), rec.normal) > 0); // absorb if rough scatter makes ray go into object
	}

	material_type get_type() const override {return material_type::Metallic;}

private:
	shared_ptr<texture> albedo;
	double roughness;
};

#endif //RAYTRACINGWEEKEND_METALLIC_H