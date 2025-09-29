#ifndef RAYTRACINGWEEKEND_TEX_PERLIN_H
#define RAYTRACINGWEEKEND_TEX_PERLIN_H
#include "../../texture.h"
#include "../generator/perlin.h"

class tex_perlin : public texture
{
public:
	tex_perlin(double scale, double turbulence = 1) : _color(make_shared<tex_color>(1,1,1)), scale(scale), turbulence(turbulence) {}
	tex_perlin(std::shared_ptr<texture> tex, double scale, double turbulence = 1) : _color(tex), scale(scale), turbulence(turbulence) {}

	tex_perlin(std::string name, std::shared_ptr<texture> tex) : tex_perlin(tex, 1, 1)
	{
		this->name = std::move(name);
	}

	color value(double u, double v, const point3& p) const override
	{
		return color::half * (1 + std::sin(scale * p.z() + 10 * noise.turbulence(p, turbulence))) * _color->value(u, v, p);
	}

	texture_type get_type() const override {return Perlin;}

	bool inspector_ui(viewport& _viewport, scene& _scene) override
	{
		bool modified = false;
		if (texture_slot("Color", _color, this, _scene))
			modified = true;

		if (ImGui::DragDouble("Noise scale", &scale, 0.5)) modified = true;

		if (ImGui::DragDouble("Noise Turbulence", &turbulence, 0.5)) modified = true;
		ImGui::SetItemTooltip("Along the Z axis only.");

		if (modified)
			_viewport.mark_scene_dirty();

		return modified;
	}
private:
	shared_ptr<texture> _color;

	perlin noise;
	double scale;
	double turbulence;
};

#endif //RAYTRACINGWEEKEND_TEX_PERLIN_H