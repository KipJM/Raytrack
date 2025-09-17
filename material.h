#ifndef RAYTRACINGWEEKEND_MATERIAL_H
#define RAYTRACINGWEEKEND_MATERIAL_H


#include "imgui/imgui.h"
#include "hittable.h"
#include "misc.h"
// #include "scene.h"


enum material_type
{
	Debug_Normal,
	Diffuse,
	Emissive,
	Metallic,
	Translucent,
	Volumetric
};

class material
{
public:
	std::string name;

	virtual ~material() = default;

	virtual bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const
	{
		return false;
	}

	virtual color emitted(double u, double v, const point3& p) const
	{
		return color::zero;
	}

	virtual material_type get_type() const = 0;

	[[nodiscard]] std::string get_human_type() const
	{
		switch (get_type())
		{
		case Debug_Normal:
			return "Normals Debug";
		case Diffuse:
			return "Diffuse";
		case Emissive:
			return "Emissive";
		case Metallic:
			return "Metallic";
		case Translucent:
			return "Translucent";
		case Volumetric:
			return "Volumetric";
		}
		return "Unknown";
	}

};

#endif //RAYTRACINGWEEKEND_MATERIAL_H