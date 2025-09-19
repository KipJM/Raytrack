#ifndef RAYTRACINGWEEKEND_TEX_IMAGE_H
#define RAYTRACINGWEEKEND_TEX_IMAGE_H
#include "../../texture.h"
#include "../../rtw_image.h"

class tex_image : public texture
{
public:
	tex_image(const char* filename) : image(filename) {}

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
private:
	rtw_image image;
};

#endif //RAYTRACINGWEEKEND_TEX_IMAGE_H