#ifndef RAY_H
#define RAY_H

#include "vec3.h"

class ray
{
public:
	ray() {}

	ray (const point3& origin, const vec3& direction, double time) : orig(origin), dir(direction), tm(time) {}

	ray (const point3& origin, const vec3& direction) : ray(origin, direction, 0) {}

	ray (point3& origin, vec3& direction, double time) : orig(origin), dir(direction), tm(time) {} // Non-const

	const point3& origin() const { return orig; }
	const vec3& direction() const { return dir; }
	double time() const { return tm; }

	point3& modify_origin() { return orig; }
	vec3& modify_direction() { return dir; }
	double& modify_time() { return tm; }

	point3 at(double t) const
	{
		return orig + t*dir;
	}

private:
	point3 orig;
	vec3 dir;
	double tm;
};

#endif