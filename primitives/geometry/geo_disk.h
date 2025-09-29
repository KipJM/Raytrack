#ifndef RAYTRACINGWEEKEND_PLANE_H
#define RAYTRACINGWEEKEND_PLANE_H

#include "geo_quad.h"
#include "../../hittable.h"
#include "../../misc.h"

#include <cmath>

class geo_disk : public geo_quad
{
public:
	hittable_type get_type() const override { return hittable_type::disk; }

	/// UI Constructor
	geo_disk(std::string name, shared_ptr<material> mat) : geo_disk(point3(0,0,0), vec3(1,0,0), vec3(0,1,0), mat)
	{
		this->name = name;
	}

	geo_disk(const point3& Q, const vec3& u, const vec3& v, shared_ptr<material> mat)
		: geo_quad(Q, u, v, mat) {}

	virtual void set_bounding_box() override
	{
		auto bbox_diagonal1 = aabb(Q, Q + u + v);
		auto bbox_diagonal2 = aabb(Q + u, Q + v);
		bbox = aabb(bbox_diagonal1, bbox_diagonal2);
	}

	virtual bool is_interior(double a, double b, hit_record& rec) const override
	{
		if (std::pow(2*a - 1,2)+std::pow(2 * b - 1,2) > 1)
			return false;

		// set uv coords
		rec.u = a;
		rec.v = b;
		return true;
	}

	bool inspector_ui(viewport& _viewport, scene& _scene) override
	{
		ImGui::Text("Disks are a special type of quad, so tooltips are unchanged.");
		return geo_quad::inspector_ui(_viewport, _scene);
	}
};

#endif //RAYTRACINGWEEKEND_PLANE_H