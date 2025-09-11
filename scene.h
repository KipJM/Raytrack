//

#ifndef RAYTRACINGWEEKEND_SCENE_H
#define RAYTRACINGWEEKEND_SCENE_H
#include "bvh.h"
#include "camera.h"
#include "hittable_list.h"
#include "texture.h"

class scene
{
public:
	camera camera;

	std::vector<shared_ptr<texture>> textures;
	std::vector<shared_ptr<material>> materials;
	hittable_list world;

	void update()
	{
		if (dirty)
		{
			// Regenerate BVH
			bvh_cache = hittable_list(make_shared<bvh_node>(world));
			dirty = false;
		}

	}

	bool mark_dirty()
	{
		auto prev_dirty = dirty;
		dirty = true;
		return prev_dirty;
	}

	[[nodiscard]] bool is_dirty() const
	{
		return dirty;
	}

	hittable& get_render_scene()
	{
		if (dirty)
			update();

		return bvh_cache;
	}

private:
	bool dirty = true;
	hittable_list bvh_cache;
};


#endif //RAYTRACINGWEEKEND_SCENE_H