#ifndef RAYTRACINGWEEKEND_HITTABLE_LIST_H
#define RAYTRACINGWEEKEND_HITTABLE_LIST_H
#include "hittable.h"


class hittable_list : public hittable
{
public:
	hittable_type get_type() const override { return hittable_type::list; }

	std::vector<shared_ptr<hittable>> objects; // meta: cpp garbage collector hell :)

	hittable_list() {}
	hittable_list(shared_ptr<hittable> object) { add(object); }

	void clear() { objects.clear(); }
	void add(shared_ptr<hittable> object)
	{
		objects.push_back(object);
		bbox = aabb(bbox, object->bounding_box());
	}

	void remove(int index)
	{
		objects.erase(objects.begin() + index);

		rebuild_aabb();
	}

	void rebuild_aabb()
	{
		// recalculate bounding box
		bbox = aabb();

		for (shared_ptr<hittable>& object : objects)
		{
			bbox = aabb(bbox, object->bounding_box());
		}
	}


	bool hit(const ray& r, interval ray_t, hit_record& rec) const override
	{
		hit_record temp_rec;
		bool hit_any = false;
		auto closest_so_far = ray_t.max;

		for (const auto& object : objects)
		{
			if (object->hit(r, interval(ray_t.min, closest_so_far), temp_rec))
			{
				hit_any = true;
				closest_so_far = temp_rec.t;
				rec = temp_rec;
			}
		}

		return hit_any;
	}

	aabb bounding_box() const override
	{
		return bbox;
	}

	bool inspector_ui(viewport& viewport, scene& scene) override;

private:
	aabb bbox;

	int selection; // for UI
};

#endif //RAYTRACINGWEEKEND_HITTABLE_LIST_H