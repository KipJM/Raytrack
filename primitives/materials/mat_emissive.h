#ifndef RAYTRACINGWEEKEND_EMISSIVE_H
#define RAYTRACINGWEEKEND_EMISSIVE_H
#include "../../texture.h"
#include "../textures/tex_color.h"
#include "../../material.h"

class mat_emissive : public material
{
public:
	mat_emissive(shared_ptr<texture> emission) : tex(emission) {}
	mat_emissive(const color& emission) : tint(emission) {}
	mat_emissive(shared_ptr<texture> emission, const color& tint) : tex(emission), tint(tint) {}

	/// UI
	mat_emissive(std::string name, shared_ptr<texture> tex) : mat_emissive(tex, color::one)
	{
		this->name = name;
	}

	color emitted(double u, double v, const point3& p) const override
	{
		if (tex == nullptr)
		{
			return tint;
		} else
		{
			return tex -> value(u, v, p) * tint;
		}
	}

	material_type get_type() const override {return material_type::Emissive;}

	bool inspector_ui(viewport& viewport, scene& scene) override
	{
		bool modified = false;

		if (texture_slot("Color", tex, scene)) modified = true;
		ImGui::SetItemTooltip("What color to emit. Must be non-black to be emissive, despite the tint.");

		auto tint_buf = tint.get_float();
		if (ImGui::ColorEdit3("Tint", tint_buf, ImGuiColorEditFlags_Float | ImGuiColorEditFlags_HDR))
		{
			tint.set_float(tint_buf);
			modified = true;
		}
		ImGui::SetItemTooltip("This value will be multiplied with the color as the final emission. Note that components can go over 1 (HDR).");

		if (modified)
			viewport.mark_scene_dirty();

		return modified;
	}

private:
	shared_ptr<texture> tex;
	color tint = color::one;
};

#endif //RAYTRACINGWEEKEND_EMISSIVE_H