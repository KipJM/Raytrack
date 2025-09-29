#ifndef RAYTRACINGWEEKEND_MATERIAL_H
#define RAYTRACINGWEEKEND_MATERIAL_H

#include "misc.h"


class viewport;
class scene;
class hit_record;

enum material_type
{
	Debug_Normal,
	Diffuse,
	Emissive,
	Metallic,
	Translucent,
	Volumetric
};

[[nodiscard]] inline std::string material_get_human_type(material_type type)
{
	switch (type)
	{
	case Debug_Normal:
		return "Normals (DEBUG)";
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

	/// UI: Displays mat-specific inspector UI
	///
	/// Returns: True if mat is modified
	virtual bool inspector_ui(viewport& _viewport, scene& _scene) {return false;}

	virtual material_type get_type() const = 0;

	[[nodiscard]] std::string get_human_type() const
	{
		return material_get_human_type(get_type());
	}

};

#endif //RAYTRACINGWEEKEND_MATERIAL_H