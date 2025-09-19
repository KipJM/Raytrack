#ifndef RAYTRACINGWEEKEND_SPHERE_H
#define RAYTRACINGWEEKEND_SPHERE_H

#include "../../hittable.h"
#include "../../imgui/imgui.h"
#include "../../ui_components.h"

class geo_sphere : public hittable
{
public:
	hittable_type get_type() const override { return hittable_type::sphere; }

	/// UI Constructor
	geo_sphere(std::string name, shared_ptr<material> mat) : geo_sphere(point3(0,0,0), 1, mat)
	{
		this->name = name;
	}

	/// Static
	geo_sphere(const point3 center, double radius, shared_ptr<material> mat) :
		center(center), radius(std::fmax(0,radius)), mat(mat)
	{
		auto rvec = vec3::one * radius;
		bbox = aabb(center - rvec, center + rvec);
	}

	/// Dynamic (UNUSED)
	// geo_sphere(const point3& center_start, const point3& center_end, double radius, shared_ptr<material> mat) :
	// 	center(center_start, center_end - center_start), radius(std::fmax(0,radius)), mat(mat)
	// {
	// 	auto rvec = vec3::one * radius;
	//
	// 	aabb box0(center.at(0) - rvec, center.at(0) + rvec);
	// 	aabb box1(center.at(1) - rvec, center.at(1) + rvec);
	// 	bbox = aabb(box0, box1);
	// }


	bool hit(const ray& r, interval ray_t, hit_record& rec) const override
	{
		// point3 center_current = center.at(r.time());
		point3 center_current = center;
		vec3 oc = center_current - r.origin();
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
		vec3 outward_normal = (rec.p - center_current) / radius;
		rec.set_face_normal(r, outward_normal);
		get_uv(outward_normal, rec.u, rec.v);
		rec.mat = mat;

		return true;
	}

	aabb bounding_box() const override { return bbox; }

	bool inspector_ui(viewport& viewport, scene& scene) override
	{
		bool changed = false;
		if (ImGui::DragDouble3("Center", center.e))
		{
			changed = true;
		}
		ImGui::SetItemTooltip("World position of the center of the sphere.");

		if (ImGui::DragDouble("Radius", &radius))
		{
			changed = true;
		}
		ImGui::SetItemTooltip("World distance from the surface to the center of your sphere. Note that objects are hollow.");


		if (material_slot("Material", mat, scene))
		{
			changed = true;
		}

		if (changed)
		{
			viewport.mark_scene_dirty();
			// Recalc bbox
			auto rvec = vec3::one * radius;
			bbox = aabb(center - rvec, center + rvec);
		}

		return changed;
	}

private:
	point3 center;
	double radius;
	shared_ptr<material> mat;
	aabb bbox;

	/// Sets U, V based on normal (point on unit sphere)
	static void get_uv(const point3& p, double& u, double& v)
	{
		auto theta = std::acos(-p.y());
		auto phi = std::atan2(-p.z(), p.x()) + pi;

		u = phi / (2 * pi);
		v = theta / pi;
	}
};

#endif //RAYTRACINGWEEKEND_SPHERE_H