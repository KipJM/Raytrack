#ifndef RAYTRACINGWEEKEND_QUAD_H
#define RAYTRACINGWEEKEND_QUAD_H
#include "../../hittable.h"
#include "../../ui_components.h"

class geo_quad : public hittable
{
public:
	hittable_type get_type() const override {return hittable_type::quad;}

	/// UI Constructor
	geo_quad(std::string name, shared_ptr<material> mat) : geo_quad(point3(0,0,0), vec3(1,0,0), vec3(0,1,0), mat)
	{
		this->name = name;
	}

	geo_quad(const point3& Q, const vec3& u, const vec3& v, shared_ptr<material> mat)
		: Q(Q), u(u), v(v), mat(mat)
	{
		auto n = cross(u, v);
		normal = unit_vector(n);
		D = dot(normal, Q);
		w = n / dot(n,n);

		set_bounding_box();
	}

	virtual void set_bounding_box()
	{
		auto bbox_diagonal1 = aabb(Q, Q + u + v);
		auto bbox_diagonal2 = aabb(Q + u, Q + v);
		bbox = aabb(bbox_diagonal1, bbox_diagonal2);
	}

	aabb bounding_box() const override { return bbox; }

	bool hit(const ray& r, interval ray_t, hit_record& rec) const override
	{
		auto denom = dot(normal, r.direction());

		if (std::fabs(denom) < 1e-8) // basically parallel to plane
			return false;

		auto t = (D - dot(normal, r.origin())) / denom;
		if (!ray_t.contains(t))
			return false; // Outside hit region

		auto intersection = r.at(t);
		vec3 planar_hit_vector = intersection - Q;
		auto alpha = dot(w, cross(planar_hit_vector, v));
		auto beta = dot(w, cross(u, planar_hit_vector)); // Funky vector math that can be done with matrix instead

		// check if on quad
		if (!is_interior(alpha, beta, rec))
			return false;

		rec.t = t;
		rec.p = intersection;
		rec.mat = mat;
		rec.set_face_normal(r, normal);

		return true;
	}

	virtual bool is_interior(double a, double b, hit_record& rec) const
	{
		interval unit_interval = interval(0,1);
		if (!unit_interval.contains(a) || !unit_interval.contains(b))
			return false; // not on quad

		// set uv coords
		rec.u = a;
		rec.v = b;
		return true;
	}

	bool inspector_ui(viewport& _viewport, scene& _scene) override
	{
		bool modified = false;

		modified += ImGui::DragDouble3("Position", Q.e);
		ImGui::SetItemTooltip("The position of the bottom-left point of the quad.");

		modified += ImGui::DragDouble3("U direction", u.e);
		ImGui::SetItemTooltip("The local position of the bottom-right point of the quad. If you want a rectangle that is 1 unit wide, enter 1,0,0.");

		modified += ImGui::DragDouble3("V direction", v.e);
		ImGui::SetItemTooltip("The local position of the top-left point of the quad. If you want a rectangel that is 1 unit tall, enter 0,1,0.");

		modified += material_slot("Material", mat, _scene);

		if (modified)
		{
			// Technically material doesn't need this to update, but it's so cheap to update it's not a big deal
			auto n = cross(u, v);
			normal = unit_vector(n);
			D = dot(normal, Q);
			w = n / dot(n,n);

			set_bounding_box();

			_viewport.mark_scene_dirty();
		}
		return modified;
	}

	/// Internal function to update the mesh data of each side of a cube
	/// We assume that someone else will notify the viewport of an update
	void cube_update_props(vec3 q, vec3 u, vec3 v)
	{
		this->Q = q;
		this->u = u;
		this->v = v;

		auto n = cross(u, v);
		normal = unit_vector(n);
		D = dot(normal, Q);
		w = n / dot(n,n);

		set_bounding_box();
	}

	/// Internal function to update material data of each sie of a cube.
	/// The pointer is therefore shared since it's managed by the cube.
	void cube_set_mat(shared_ptr<material>& material)
	{
		this->mat = material;
	}

protected:
	point3 Q;
	vec3 u, v, w;
	shared_ptr<material> mat;
	aabb bbox;

	vec3 normal;
	double D;

};

#endif //RAYTRACINGWEEKEND_QUAD_H