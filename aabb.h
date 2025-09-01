#ifndef RAYTRACINGWEEKEND_AABB_H
#define RAYTRACINGWEEKEND_AABB_H

#include "rtweekend.h"

class aabb
{
public:
	interval x, y, z;

	aabb() {} // default: empty AABB

	aabb(const interval& x, const interval& y, const interval& z) : x(x), y(y), z(z) {}

	/// Generate AABB that encompass the two points
	aabb (const point3& a, const point3& b)
	{
		x = (a.x() <= b.x()) ? interval(a.x(), b.x()) : interval(b.x(), a.x());
		y = (a.y() <= b.y()) ? interval(a.y(), b.y()) : interval(b.y(), a.y());
		z = (a.z() <= b.z()) ? interval(a.z(), b.z()) : interval(b.z(), a.z());
	}

	/// Generate AABB that encompass both AABBs
	aabb (const aabb& box_a, const aabb& box_b)
	{
		x = interval(box_a.x, box_b.x);
		y = interval(box_a.y, box_b.y);
		z = interval(box_a.z, box_b.z);
	}

	const interval& axis_interval(int n) const
	{
		switch (n)
		{
		case 0: return x;
		case 1: return y;
		case 2: return z;
		default:
			throw std::domain_error("Requested dimensional complexity exceeds renderer limit (3)");
		}
	}

	bool hit(const ray& r, interval ray_t) const
	{
		const point3& ray_orig = r.origin();
		const vec3& ray_dir = r.direction();


		for (int axis = 0; axis < 3; axis++)
		{
			const interval& ax = axis_interval(axis);
			const double adinv = 1.0 / ray_dir[axis];

			auto t0 = (ax.min - ray_orig[axis]) * adinv;
			auto t1 = (ax.max - ray_orig[axis]) * adinv;

			if (t0 < t1)
			{
				if (t0 > ray_t.min) ray_t.min = t0;
				if (t1 < ray_t.max) ray_t.max = t1;
			} else
			{
				if (t1 > ray_t.min) ray_t.min = t1;
				if (t0 < ray_t.max) ray_t.max = t0;
			}

			if (ray_t.max <= ray_t.min)
				return false;

		}
		return true;
	}


	/// Returns the index (x,y,z) of the longest axis of the AABB
	int longest_axis() const
	{
		if (x.size() < y.size())
		{
			return y.size() < z.size() ? 2 : 1;
		}else
		{
			return x.size() < z.size() ? 2 : 0;
		}
	}

	static const aabb empty, universe;
};

const aabb aabb::empty = aabb(interval::empty, interval::empty, interval::empty);
const aabb aabb::universe = aabb(interval::universe, interval::universe, interval::universe);

#endif //RAYTRACINGWEEKEND_AABB_H