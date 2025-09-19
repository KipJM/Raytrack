#ifndef RAYTRACINGWEEKEND_TEXTURE_H
#define RAYTRACINGWEEKEND_TEXTURE_H
#include "color.h"
#include "vec3.h"

enum texture_type
{
	Color,
	Image,
	Perlin,
	UV,
	Checker
};

[[nodiscard]] inline std::string texture_get_human_type(texture_type type)
{
	switch (type)
	{
	case Color:
		return "Solid color";
	case Image:
		return "Image texture";
	case Perlin:
		return "Perlin noise";
	case UV:
		return "UV (DEBUG)";
	case Checker:
		return "Checker pattern";
	}
	return "Unknown";
}

class texture
{
public:
	std::string name;

	virtual ~texture() = default;

	virtual color value(double u, double v, const point3& p) const = 0;

	virtual texture_type get_type() const = 0;

	std::string get_human_type()
	{
		return texture_get_human_type(get_type());
	}
};


#endif //RAYTRACINGWEEKEND_TEXTURE_H