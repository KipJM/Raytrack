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

class dielectric : public material
{
public:
	dielectric(double refraction_index) : refraction_index(refraction_index) {}

	bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override
	{
		attenuation = color(1, 1, 1); //TODO
		double ri = rec.front_face ? (1.0/refraction_index) : refraction_index; // Air refraction index is basically 1

		vec3 unit_direction = unit_vector(r_in.direction());

		double cos_theta = std::fmin(dot(-unit_direction, rec.normal), 1.0);
		double sin_theta = std::sqrt(1.0 - cos_theta * cos_theta);

		bool cannot_refract = ri * sin_theta > 1.0;

		vec3 direction;
		if (cannot_refract || reflectance(cos_theta, ri) > rand_double())
		{
			direction = reflect(unit_direction, rec.normal);
		}else
		{
			direction = refract(unit_direction, rec.normal, ri);
		}

		scattered = ray(rec.p, direction);
		return true;
	}

private:
	double refraction_index;

	static double reflectance(double cosine, double refraction_index)
	{
		// schlick approximation
		auto r0 = (1 - refraction_index) / (1 + refraction_index);
		r0 *= r0; // square
		return r0 + (1-r0)*std::pow((1-cosine), 5);
	}
};

class normal_debug : public material
{
public:
	normal_debug() : internal_lambert(lambert(color(1,1,1))){}

	bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override
	{
		internal_lambert.scatter(r_in, rec, attenuation, scattered);
		attenuation = color(unit_vector(rec.normal));
		return true;
	}

private:
	lambert internal_lambert;
};

#endif //RAYTRACINGWEEKEND_MATERIAL_H