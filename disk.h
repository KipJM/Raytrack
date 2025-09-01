#ifndef RAYTRACINGWEEKEND_PLANE_H
#define RAYTRACINGWEEKEND_PLANE_H

#include "hittable.h"
#include "rtweekend.h"

class disk : public hittable
{
public:
	disk(double height, double radius, shared_ptr<material> mat):
		height(height), radius_sqr(radius * radius), mat(mat)
	{
		interval horiz(-radius, radius);
		bbox = aabb(horiz, interval(height, height).expand(0.01), horiz);
	}

	bool hit(const ray& r, interval ray_t, hit_record& rec) const override
	{
		if (r.direction().y() > 1e-16 ) return false;

		auto root = (height - r.origin().y()) / r.direction().y();

		auto pos = r.at(root);
		if (pos.x() * pos.x() + pos.z() * pos.z() > radius_sqr) return false;

		rec.t = root;
		rec.p = pos;
		rec.set_face_normal(r, vec3(0,1,0));
		rec.mat = mat;
		return true;
	}

	aabb bounding_box() const override
	{
		return bbox;
	}

private:
	double height;
	double radius_sqr;
	shared_ptr<material> mat;
	aabb bbox;
};

#endif //RAYTRACINGWEEKEND_PLANE_H