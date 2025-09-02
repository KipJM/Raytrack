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


	void render()
	{
		if (dirty)
		{
			// Regenerate BVH
			bvh_cache = hittable_list(make_shared<bvh_node>(world));
			dirty = false;
		}

		camera.render(bvh_cache);

	}

	void mark_dirty()
	{
		dirty = true;
	}

private:
	bool dirty = true;
	hittable_list bvh_cache;
};


#endif //RAYTRACINGWEEKEND_SCENE_H