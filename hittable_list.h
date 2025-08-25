//
// Created by mayua on 2025/08/24.
//

#ifndef RAYTRACINGWEEKEND_HITTABLE_LIST_H
#define RAYTRACINGWEEKEND_HITTABLE_LIST_H

#include "hittable.h"

#include <vector>


class hittable_list : public hittable
{
public:
	std::vector<shared_ptr<hittable>> objects; // meta: cpp garbage collector hell :)

	hittable_list() {}
	hittable_list(shared_ptr<hittable> object) { add(object); }

	void clear() { objects.clear(); }
	void add(shared_ptr<hittable> object) { objects.push_back(object); }


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
};

#endif //RAYTRACINGWEEKEND_HITTABLE_LIST_H