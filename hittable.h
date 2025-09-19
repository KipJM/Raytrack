#ifndef RAYTRACINGWEEKEND_HITTABLE_H
#define RAYTRACINGWEEKEND_HITTABLE_H
#include "aabb.h"
#include "material.h"

class scene;
class viewport;

enum hittable_type
{
	cube,
	disk,
	quad,
	sphere,
	list,
	volume,
	mover,
	rotator,
	bvh
};

class hit_record
{
public:
	point3 p;
	vec3 normal;
	shared_ptr<material> mat;
	double t;

	double u;
	double v;

	bool front_face;

	/// Generate hit record normal vector, will point against ray\n
	/// REQ: `outward_normal` must be unit length
	void set_face_normal(const ray& r, const vec3& outward_normal)
	{
		front_face = dot(r.direction(), outward_normal) < 0;
		normal = front_face ? outward_normal : -outward_normal;
	}
};

class hittable
{
public:
	std::string name;

	virtual ~hittable() = default;

	virtual bool hit(const ray& r, interval ray_t, hit_record& rec) const = 0;

	virtual aabb bounding_box() const = 0;

	virtual hittable_type get_type() const = 0;

	[[nodiscard]] std::string get_human_type() const
	{
		switch (get_type())
		{
		case cube:
			return "Cube";

		case disk:
			return "Disk";

		case quad:
			return "Quad";

		case sphere:
			return "Sphere";

		case list:
			return "Compound";

		case volume:
			return "Volume";

		case mover:
			return "Translated Object";

		case rotator:
			return "Rotated Object";

		case bvh:
			return "BVH-Optimized Node";
		}

		return "Unknown";
	}

	virtual bool inspector_ui(viewport& viewport, scene& scene) {return false;}

};

[[nodiscard]] inline std::string hittable_get_human_type(hittable_type type)
{
	switch (type)
	{
	case cube:
		return "Cube";

	case disk:
		return "Disk";

	case quad:
		return "Quad";

	case sphere:
		return "Sphere";

	case list:
		return "Compound";

	case volume:
		return "Volume";

	case mover:
		return "Translated Object";

	case rotator:
		return "Rotated Object";

	case bvh:
		return "BVH-Optimized Node";
	}

	return "Unknown";
}

#endif //RAYTRACINGWEEKEND_HITTABLE_H