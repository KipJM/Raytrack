#ifndef RAYTRACINGWEEKEND_TRANSFORMERS_H
#define RAYTRACINGWEEKEND_TRANSFORMERS_H
#include <utility>

#include "hittable.h"

class trn_move : public hittable
{
public:
	hittable_type get_type() const override {return hittable_type::mover;}

	trn_move(shared_ptr<hittable> object, const vec3& offset) : object(object), offset(offset)
	{
		name = object->name + " (Translated)";
		bbox = object->bounding_box() + offset;
	}

	bool hit(const ray& r, interval ray_t, hit_record& rec) const override
	{
		// Move the ray in opposite direction by offset
		ray offset_r(r.origin() - offset, r.direction(), r.time());

		// hit record is transferred up automatically
		if (!object->hit(offset_r, ray_t, rec))
		{
			return false;
		}

		rec.p += offset;

		return true;
	}

	aabb bounding_box() const override { return bbox; }

	bool inspector_ui(viewport& viewport, scene& scene) override
	{
		// TODO
		return false;
	}
private:
	shared_ptr<hittable> object;
	vec3 offset;
	aabb bbox;
};



// Rotation (why not matrix ahhh)



class trn_rotate_x : public hittable
{
public:
	hittable_type get_type() const override {return hittable_type::rotator;}

	/// in degrees
	trn_rotate_x(shared_ptr<hittable> object, double angle) : object(object), angle(angle)
	{
		name = object->name + " (X Rotated)";

		auto radians = deg_to_rad(angle);
		sin_theta = std::sin(radians);
		cos_theta = std::cos(radians);
		bbox = object->bounding_box();

		point3 min( infinity, infinity, infinity);
		point3 max(-infinity, -infinity, -infinity);

		for (int i = 0; i < 2; i++)
		{
			for (int j = 0; j < 2; j++)
			{
				for (int k = 0; k < 2; k++)
				{
					auto x = i*bbox.x.max + (1-i) * bbox.x.min;
					auto y = j*bbox.y.max + (1-j) * bbox.y.min;
					auto z = k*bbox.z.max + (1-k) * bbox.z.min;

					// auto newx =  cos_theta * x + sin_theta * z;
					auto newy = cos_theta * y - sin_theta * z;
					auto newz = sin_theta * y + cos_theta * z;

					vec3 tester(x, newy, newz);

					for (int c = 0; c < 3; c++)
					{
						min[c] = std::fmin(min[c], tester[c]);
						max[c] = std::fmax(max[c], tester[c]);
					}
				}
			}
		}

		bbox = aabb(min, max);
	}

	// maybe should've used matrix instead
	bool hit(const ray& r, interval ray_t, hit_record& rec) const override
	{
		if (angle == 0) return object->hit(r, ray_t, rec);

		// ray: world space --> object space
		auto origin = point3(
			r.origin().x(),
			(cos_theta*r.origin().y()) + (sin_theta * r.origin().z()),
			(-sin_theta * r.origin().y()) + (cos_theta * r.origin().z())
		);

		auto direction = vec3(
			r.direction().x(),
			(cos_theta*r.direction().y()) + (sin_theta * r.direction().z()),
			(-sin_theta * r.direction().y()) + (cos_theta * r.direction().z())
		);

		ray rotated_r(origin, direction, r.time());

		if (!object->hit(rotated_r, ray_t, rec))
			return false;

		// intersection: object space --> world space

		rec.p = point3(
			rec.p.x(),
			(cos_theta * rec.p.y()) - (sin_theta * rec.p.z()),
			(sin_theta * rec.p.y()) + (cos_theta * rec.p.z())
		);

		rec.normal = vec3(
			rec.normal.x(),
			(cos_theta * rec.normal.y()) - (sin_theta * rec.normal.z()),
			(sin_theta * rec.normal.y()) + (cos_theta * rec.normal.z())
		);

		return true;
	}

	aabb bounding_box() const override { return bbox; }

private:
	shared_ptr<hittable> object;
	double angle;
	double sin_theta;
	double cos_theta;
	aabb bbox;
};



class trn_rotate_y : public hittable
{
public:
	hittable_type get_type() const override {return hittable_type::rotator;}

	/// in degrees
	trn_rotate_y(shared_ptr<hittable> object, double angle) : object(object), angle(angle)
	{
		name = object->name + " (Y Rotated)";

		auto radians = deg_to_rad(angle);
		sin_theta = std::sin(radians);
		cos_theta = std::cos(radians);
		bbox = object->bounding_box();

		point3 min( infinity, infinity, infinity);
		point3 max(-infinity, -infinity, -infinity);

		for (int i = 0; i < 2; i++)
		{
			for (int j = 0; j < 2; j++)
			{
				for (int k = 0; k < 2; k++)
				{
					auto x = i*bbox.x.max + (1-i) * bbox.x.min;
					auto y = j*bbox.y.max + (1-j) * bbox.y.min;
					auto z = k*bbox.z.max + (1-k) * bbox.z.min;

					auto newx =  cos_theta * x + sin_theta * z;
					auto newz = -sin_theta * x + cos_theta * z;

					vec3 tester(newx, y, newz);

					for (int c = 0; c < 3; c++)
					{
						min[c] = std::fmin(min[c], tester[c]);
						max[c] = std::fmax(max[c], tester[c]);
					}
				}
			}
		}

		bbox = aabb(min, max);
	}

	// maybe should've used matrix instead
	bool hit(const ray& r, interval ray_t, hit_record& rec) const override
	{
		if (angle == 0) return object->hit(r, ray_t, rec);

		// ray: world space --> object space
		auto origin = point3(
			(cos_theta * r.origin().x()) - (sin_theta * r.origin().z()),
			r.origin().y(),
			(sin_theta * r.origin().x()) + (cos_theta * r.origin().z())
		);

		auto direction = vec3(
			(cos_theta * r.direction().x()) - (sin_theta * r.direction().z()),
			r.direction().y(),
			(sin_theta * r.direction().x()) + (cos_theta * r.direction().z())
		);

		ray rotated_r(origin, direction, r.time());

		if (!object->hit(rotated_r, ray_t, rec))
			return false;

		// intersection: object space --> world space

		rec.p = point3(
			(cos_theta * rec.p.x()) + (sin_theta * rec.p.z()),
			rec.p.y(),
			(-sin_theta * rec.p.x()) + (cos_theta * rec.p.z())
		);

		rec.normal = vec3(
			(cos_theta * rec.normal.x()) + (sin_theta * rec.normal.z()),
			rec.normal.y(),
			(-sin_theta * rec.normal.x()) + (cos_theta * rec.normal.z())
		);

		return true;
	}

	aabb bounding_box() const override { return bbox; }
private:
	shared_ptr<hittable> object;
	double angle;
	double sin_theta;
	double cos_theta;
	aabb bbox;
};



class trn_rotate_z : public hittable
{
public:
	hittable_type get_type() const override {return hittable_type::rotator;}

	/// in degrees
	trn_rotate_z(shared_ptr<hittable> object, double angle) : object(object), angle(angle)
	{
		name = object->name + " (Z Rotated)";

		auto radians = deg_to_rad(angle);
		sin_theta = std::sin(radians);
		cos_theta = std::cos(radians);
		bbox = object->bounding_box();

		point3 min( infinity, infinity, infinity);
		point3 max(-infinity, -infinity, -infinity);

		for (int i = 0; i < 2; i++)
		{
			for (int j = 0; j < 2; j++)
			{
				for (int k = 0; k < 2; k++)
				{
					auto x = i*bbox.x.max + (1-i) * bbox.x.min;
					auto y = j*bbox.y.max + (1-j) * bbox.y.min;
					auto z = k*bbox.z.max + (1-k) * bbox.z.min;

					// auto newx =  cos_theta * x + sin_theta * z;
					auto newx = cos_theta * x - sin_theta * y;
					auto newy = sin_theta * x + cos_theta * y;

					vec3 tester(newx, newy, z);

					for (int c = 0; c < 3; c++)
					{
						min[c] = std::fmin(min[c], tester[c]);
						max[c] = std::fmax(max[c], tester[c]);
					}
				}
			}
		}

		bbox = aabb(min, max);
	}

	// maybe should've used matrix instead
	bool hit(const ray& r, interval ray_t, hit_record& rec) const override
	{
		if (angle == 0) return object->hit(r, ray_t, rec);

		// ray: world space --> object space
		auto origin = point3(
			(cos_theta * r.origin().x()) + (sin_theta * r.origin().y()),
			(-sin_theta*r.origin().x()) + (cos_theta * r.origin().y()),
			r.origin().z()
		);

		auto direction = vec3(
			(cos_theta * r.direction().x()) + (sin_theta * r.direction().y()),
			(-sin_theta*r.direction().x()) + (cos_theta * r.direction().y()),
			r.direction().z()
		);

		ray rotated_r(origin, direction, r.time());

		if (!object->hit(rotated_r, ray_t, rec))
			return false;

		// intersection: object space --> world space

		rec.p = point3(
			(cos_theta * rec.p.x()) - (sin_theta * rec.p.y()),
			(sin_theta * rec.p.x()) + (cos_theta * rec.p.y()),
			rec.p.z()
		);

		rec.normal = vec3(
			(cos_theta * rec.normal.x()) - (sin_theta * rec.normal.y()),
			(sin_theta * rec.normal.x()) + (cos_theta * rec.normal.y()),
			rec.normal.z()
		);

		return true;
	}

	aabb bounding_box() const override { return bbox; }
private:
	shared_ptr<hittable> object;
	double angle;
	double sin_theta;
	double cos_theta;
	aabb bbox;
};


// Full rotation
/// Order: object -> X -> Y -> Z -> output
class trn_rotate : public hittable {
public:
	hittable_type get_type() const override {return hittable_type::rotator;}

	trn_rotate(shared_ptr<hittable> object, vec3 angle) : original(object)
	{
		name = object->name + " (Rotated)";

		rot_x = make_shared<trn_rotate_x>(object, angle.x());
		rot_y = make_shared<trn_rotate_y>(rot_x, angle.y());
		rot_z = make_shared<trn_rotate_z>(rot_y, angle.z());
	}

	bool hit(const ray& r, interval ray_t, hit_record& rec) const override
	{
		return rot_z->hit(r, ray_t, rec);
	}

	aabb bounding_box() const override
	{
		return rot_z->bounding_box();
	}

	bool inspector_ui(viewport& viewport, scene& scene) override
	{
		// TODO
		return false;
	}

private:
	shared_ptr<hittable> original;
	shared_ptr<trn_rotate_x> rot_x;
	shared_ptr<trn_rotate_y> rot_y;
	shared_ptr<trn_rotate_z> rot_z;
};


#endif //RAYTRACINGWEEKEND_TRANSFORMERS_H