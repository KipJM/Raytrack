#ifndef RAYTRACINGWEEKEND_GEO_CUBE_H
#define RAYTRACINGWEEKEND_GEO_CUBE_H

#include "../../hittable.h"
#include "../../hittable_list.h"
#include "geo_quad.h"

inline shared_ptr<hittable_list> cube(const point3& a, const point3& b, shared_ptr<material> mat)
{
	auto sides = make_shared<hittable_list>();

	// construct opposite vertices with min and max coordinates
	auto min = point3(std::fmin(a.x(), b.x()), std::fmin(a.y(), b.y()), std::fmin(a.z(), b.z()));
	auto max = point3(std::fmax(a.x(), b.x()), std::fmax(a.y(), b.y()), std::fmax(a.z(), b.z()));

	auto dx = vec3(max.x() - min.x(), 0, 0);
	auto dy = vec3(0, max.y() - min.y(), 0);
	auto dz = vec3(0, 0, max.z() - min.z());

	sides->add(make_shared<geo_quad>(point3(min.x(), min.y(), max.z()), dx, dy, mat)); // front
	sides->add(make_shared<geo_quad>(point3(max.x(), min.y(), max.z()),-dz, dy, mat)); // right
	sides->add(make_shared<geo_quad>(point3(max.x(), min.y(), min.z()),-dx, dy, mat)); // back
	sides->add(make_shared<geo_quad>(point3(min.x(), min.y(), min.z()), dz, dy, mat)); // left
	sides->add(make_shared<geo_quad>(point3(min.x(), max.y(), max.z()), dx,-dz, mat)); // top
	sides->add(make_shared<geo_quad>(point3(min.x(), min.y(), min.z()), dx, dz, mat)); // bottom

	return sides;

}

#endif //RAYTRACINGWEEKEND_GEO_CUBE_H