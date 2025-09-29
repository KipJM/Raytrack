#ifndef RAYTRACINGWEEKEND_SOLID_COLOR_H
#define RAYTRACINGWEEKEND_SOLID_COLOR_H
#include <utility>

#include "../../texture.h"
#include "../../viewport.h"

class tex_color : public texture
{
	public:
	tex_color(const color& albedo) : albedo(albedo) {}

	tex_color(double red, double green, double blue) : tex_color(color(red, green, blue)) {}

	/// UI
	tex_color(std::string name, color albedo) : tex_color(albedo)
	{
		this->name = std::move(name);
	}


	color value(double u, double v, const point3& p) const override
	{
		return albedo;
	}

	texture_type get_type() const override {return Color;}

	bool inspector_ui(viewport& _viewport, scene& _scene) override
	{
		auto col_buf = albedo.get_float();
		if (ImGui::ColorEdit3("Color", col_buf))
		{
			albedo.set_float(col_buf);
			_viewport.mark_scene_dirty();
			return true;
		}
		return false;
	}

private:
	color albedo;
};

#endif //RAYTRACINGWEEKEND_SOLID_COLOR_H