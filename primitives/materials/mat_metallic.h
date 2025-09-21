#ifndef RAYTRACINGWEEKEND_METALLIC_H
#define RAYTRACINGWEEKEND_METALLIC_H

#include "../../texture.h"
#include "../textures/tex_color.h"
#include "../../material.h"

class mat_metallic : public material
{
public:
	mat_metallic(const color& albedo, double roughness) : albedo(make_shared<tex_color>(albedo)), roughness(roughness < 1? roughness : 1) {}; // weird cap on roughness :|
	mat_metallic(shared_ptr<texture> albedo, double roughness) : albedo(albedo), roughness(roughness < 1 ? roughness : 1) {}

	/// UI
	mat_metallic(std::string name, shared_ptr<texture> tex) : albedo(tex), roughness(0.5)
	{
		this->name = name;
	}

	bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override
	{
		vec3 reflected = reflect(r_in.direction(), rec.normal);
		reflected = unit_vector(reflected) + (roughness * rand_unit_vector());
		scattered = ray(rec.p, reflected, r_in.time());
		attenuation = albedo->value(rec.u, rec.v, rec.p);
		return (dot(scattered.direction(), rec.normal) > 0); // absorb if rough scatter makes ray go into object
	}

	material_type get_type() const override {return material_type::Metallic;}

	bool inspector_ui(viewport& viewport, scene& scene) override
	{
		bool modified = false;

		if (texture_slot("Albedo", albedo, scene))
			modified = true;

		if (ImGui::DragDouble("Roughness", &roughness, 0.1, 0, 1))
			modified = true;
		ImGui::SetItemTooltip("How rough is the material. 0 means perfectly smooth (mirror-like).");

		if (modified)
		{
			viewport.mark_scene_dirty();
		}
		return modified;
	}

private:
	shared_ptr<texture> albedo;
	double roughness;
};

#endif //RAYTRACINGWEEKEND_METALLIC_H