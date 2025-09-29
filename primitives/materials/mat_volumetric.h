#ifndef RAYTRACINGWEEKEND_MAT_VOLUMETRIC_H
#define RAYTRACINGWEEKEND_MAT_VOLUMETRIC_H
#include "../../material.h"
#include "../../texture.h"
#include "../textures/tex_color.h"

class mat_volumetric : public material {
public:
	mat_volumetric(const color& albedo) : tex(make_shared<tex_color>(albedo)) {}
	mat_volumetric(shared_ptr<texture> albedo) : tex(albedo) {}

	/// UI constructor
	mat_volumetric(std::string name, shared_ptr<texture> albedo) : mat_volumetric(albedo)
	{
		this->name = name;
	}

	bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override
	{
		scattered = ray(rec.p, rand_unit_vector(), r_in.time());
		attenuation = tex->value(rec.u, rec.v, rec.p);
		return true;
	}

	material_type get_type() const override {return material_type::Volumetric;}

	bool inspector_ui(viewport& _viewport, scene& _scene) override
	{
		ImGui::Text("Unexpected results may happen if you use this material on non-volumes.");

		if (texture_slot("Color", tex, _scene))
		{
			_viewport.mark_scene_dirty();
			return true;
		}
		return false;
	}

private:
	shared_ptr<texture> tex;
};

#endif //RAYTRACINGWEEKEND_MAT_VOLUMETRIC_H