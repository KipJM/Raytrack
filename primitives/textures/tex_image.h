#ifndef RAYTRACINGWEEKEND_TEX_IMAGE_H
#define RAYTRACINGWEEKEND_TEX_IMAGE_H
#include <utility>

#include "../../texture.h"
#include "../../rtw_image.h"

class tex_image : public texture
{
public:
	tex_image(const char* filename) : filename(filename), image(filename) {}

    tex_image(std::string name, const char* filename) : tex_image(filename)
	{
		this->name = std::move(name);
	}

	color value(double u, double v, const point3& p) const override
	{
		if (image.height() <= 0) return color(0,1,1); // Returns cyan if no image data

		u = interval(0,1).clamp(u);
		v = 1.0 - interval(0,1).clamp(v); // Flip V from image coordinates

		auto i = int(u * image.width());
		auto j = int(v * image.height());
		auto pixel = image.pixel_data(i,j);

		auto color_scale = 1.0 / 255.0;
		return color(color_scale*pixel[0], color_scale*pixel[1], color_scale*pixel[2]); // convert byte back to float
	}

	texture_type get_type() const override {return Image;}

	bool inspector_ui(viewport& viewport, scene& scene) override
	{
		ImGui::Text("An image textures cannot be modified after it\'s created.");
		ImGui::Text("To use a different image, create another image texture.");
		ImGui::Spacing();
		ImGui::TextDisabled("The properties below are read-only.");
		ImGui::BeginDisabled();

		ImGui::Text("File name: ");
		ImGui::SameLine();
		ImGui::Text(filename.c_str());

		int res[] = {image.width(), image.height()};
		ImGui::DragInt2("Resolution", res);

		ImGui::EndDisabled();
		return false;
	}
private:
	std::string filename;
	rtw_image image;
};

#endif //RAYTRACINGWEEKEND_TEX_IMAGE_H