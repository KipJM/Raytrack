//

#ifndef RAYTRACINGWEEKEND_CHECKER_H
#define RAYTRACINGWEEKEND_CHECKER_H
#include "tex_color.h"
#include "../../texture.h"

class tex_checker : public texture
{
public:
	tex_checker(double scale, shared_ptr<texture> even, shared_ptr<texture> odd) :
		inv_scale(1.0/scale), even(even), odd(odd) {}

	tex_checker(double scale, const color& c1, const color& c2) :
		tex_checker(scale, make_shared<tex_color>(c1), make_shared<tex_color>(c2)) {}

	/// UI
	tex_checker(std::string name, shared_ptr<texture> even, shared_ptr<texture> odd) : tex_checker(.5, std::move(even), std::move(odd))
	{
		this->name = std::move(name);
	}

	color value(double u, double v, const point3& p) const override
	{
		auto x_int = int(std::floor(inv_scale*u));
		auto y_int = int(std::floor(inv_scale*v));
		// auto z_int = int(std::floor(inv_scale*p.z()));

		bool is_even = (x_int + y_int) % 2 == 0;

		return is_even ? even->value(u, v, p) : odd->value(u, v, p);
	}

	texture_type get_type() const override {return Checker;}

	bool inspector_ui(viewport& viewport, scene& scene) override
	{
		bool modified = false;

		if (texture_slot("Texture A", even, this, scene))
			modified = true;
		ImGui::SetItemTooltip("The texture on even squares.");

		if (texture_slot("Texture B", odd, this, scene))
			modified = true;
		ImGui::SetItemTooltip("The texture on odd squares.");

		double scale_buf = 1.0/inv_scale;
		if (ImGui::DragDouble("Scale", &scale_buf, 1, 0.0001, 9999999))
		{
			inv_scale = 1.0/scale_buf;
			modified = true;
		}

		if (modified)
			viewport.mark_scene_dirty();

		return modified;
	}
private:
	double inv_scale;
	shared_ptr<texture> even, odd;
};

#endif //RAYTRACINGWEEKEND_CHECKER_H