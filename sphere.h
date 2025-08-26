#ifndef RAYTRACINGWEEKEND_SPHERE_H
#define RAYTRACINGWEEKEND_SPHERE_H

#include "hittable.h"

class sphere : public hittable
{
public:
	sphere(const point3& center, double radius, shared_ptr<material> mat) :
				center(center), radius(std::fmax(0,radius)), mat(mat) {}

	bool hit(const ray& r, interval ray_t, hit_record& rec) const override
	{
		vec3 oc = center - r.origin();
		auto a = r.direction().length_squared();
		auto h = dot(r.direction(), oc);
		auto c = oc.length_squared() - radius * radius;
		auto discriminant = h*h - a*c;

		if (discriminant < 0) // no hit
			return false;

		auto sqrtd = std::sqrt(discriminant);

		// Find the nearest root that's in the acceptable range of [tmin,tmax]
		auto root = (h-sqrtd) / a;
		if (!ray_t.surrounds(root))
		{
			root = (h + sqrtd) / a; // find alt root
			if (!ray_t.surrounds(root))
			{
				// both root unsatisfactory
				return false;
			}
		}

		rec.t = root;
		rec.p = r.at(rec.t);
		vec3 outward_normal = (rec.p - center) / radius;
		rec.set_face_normal(r, outward_normal);
		rec.mat = mat;

		return true;
	}

private:
	point3 center;
	double radius;
	shared_ptr<material> mat;
};

#endif //RAYTRACINGWEEKEND_SPHERE_H