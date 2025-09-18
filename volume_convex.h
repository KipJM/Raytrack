#ifndef RAYTRACINGWEEKEND_VOLUME_CONVEX_H
#define RAYTRACINGWEEKEND_VOLUME_CONVEX_H
#include "hittable.h"

#include "primitives/materials/mat_volumetric.h"

class volume_convex : public hittable
{
public:
	hittable_type get_type() const override { return hittable_type::volume; }

	volume_convex(shared_ptr<hittable> boundary, double density, const shared_ptr<material> material)
		: boundary(boundary), neg_inv_density(-1/density), phase_function(material)
	{
		name = boundary->name + " (Volume)";
	}

	bool hit(const ray& r, interval ray_t, hit_record& rec) const override
	{
		hit_record rec1, rec2;

		if (!boundary->hit(r, interval::universe, rec1))
			return false;

		if (!boundary->hit(r, interval(rec1.t+0.0001, infinity), rec2))
			return false;

		if (rec1.t < ray_t.min) rec1.t = ray_t.min;
		if (rec2.t > ray_t.max) rec2.t = ray_t.max;

		if (rec1.t >= rec2.t)
			return false;

		if (rec1.t < 0)
			rec1.t = 0;

		auto ray_length = r.direction().length();
		auto distance_in_boundary = (rec2.t - rec1.t) * ray_length;
		auto hit_distance = neg_inv_density * std::log(rand_double()); // dampening

		if (hit_distance > distance_in_boundary)
			return false;

		rec.t = rec1.t + hit_distance / ray_length;
		rec.p = r.at(rec.t);

		rec.normal = vec3::right; // arbitrary
		rec.front_face = true; // arbitrary
		rec.mat = phase_function;

		// std::wclog << "WTF";

		return true;
	}

	aabb bounding_box() const override { return boundary->bounding_box(); }

	bool inspector_ui(viewport& viewport, scene& scene) override
	{
		bool modified = false;
		modified += hittable_slot("Shape", boundary, *this, scene);
		ImGui::SetItemTooltip("The volume will be in this object\'s shape. Please do not make circular references.");

		double density = -1.0 / neg_inv_density;
		if (ImGui::DragDouble("Density", &density, 0.1, 0.0001, 200) && density > 0.0001)
		{
			modified = true;
			neg_inv_density = -1/density;
		}
		ImGui::SetItemTooltip("The density of the object is set here, not in materials. Density must be over 0.");

		modified += material_slot("Material", phase_function, scene);
		ImGui::SetItemTooltip("Using a non-volumetric material will lead to unexpected results.");

		if (modified)
			viewport.mark_scene_dirty();

		return modified;
	}

private:
	shared_ptr<hittable> boundary;
	double neg_inv_density;
	shared_ptr<material> phase_function;
};

#endif //RAYTRACINGWEEKEND_VOLUME_CONVEX_H