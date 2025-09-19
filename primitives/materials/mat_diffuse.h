#ifndef RAYTRACINGWEEKEND_LAMBERT_H
#define RAYTRACINGWEEKEND_LAMBERT_H

#include "../../texture.h"
#include "../textures/tex_color.h"
#include "../../material.h"
#include "../../ui_components.h"

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

	material_type get_type() const override {return material_type::Diffuse;}

	bool inspector_ui(viewport& viewport, scene& scene) override
	{
		bool modified = false;

		if (texture_slot("Albedo", albedo, scene))
			modified = true;

		if (modified)
			viewport.mark_scene_dirty();

		return modified;
	}

private:
	shared_ptr<texture> albedo;
};

#endif //RAYTRACINGWEEKEND_LAMBERT_H