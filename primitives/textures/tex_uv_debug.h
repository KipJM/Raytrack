//

#ifndef RAYTRACINGWEEKEND_TEX_UV_DEBUG_H
#define RAYTRACINGWEEKEND_TEX_UV_DEBUG_H
#include "../../texture.h"

class tex_uv_debug : public texture
{
public:
	tex_uv_debug() : texture() {}

	explicit tex_uv_debug(std::string name)
	{
		this->name = std::move(name);
	}

	color value(double u, double v, const point3& p) const override
	{
		return color(u,v,0);
	}

	texture_type get_type() const override {return UV;}

	bool inspector_ui(viewport& viewport, scene& scene) override
	{
		ImGui::Text("This procedural texture visualizes the UV coordinates via U=R, V=G.");
		ImGui::TextDisabled("This texture has no properties to be modified!");
		return false;
	}
};

#endif //RAYTRACINGWEEKEND_TEX_UV_DEBUG_H