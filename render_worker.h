#ifndef RAYTRACINGWEEKEND_RENDERER_H
#define RAYTRACINGWEEKEND_RENDERER_H
#include "camera.h"
#include "hittable_list.h"
#include "viewport.h"

class render_worker
{
public:

	explicit render_worker(viewport& viewport) : viewport(viewport), early_exit(false), sigkill(false)
	{
		thread = std::thread(&render_worker::render_loop, this);
	}

	~render_worker()
	{
		early_exit = true;
		sigkill = true;
	}

	void reset()
	{
		early_exit = true;
	}

	void render_loop()
	{
		while (!sigkill)
		{
			if (render(viewport.scene.camera, viewport.scene.get_render_scene()))
			{
				viewport.append_image(output);
			}
			else if (early_exit)
			{
				// reset early exit
				early_exit = false;
			}
		}
		std::clog << "thread died an honorable death" << '\n';
	}

	bool render(camera& camera, const hittable& world)
	{
		// clear render buffer
		output = std::vector<unsigned char>();
		return camera.render(world, output, early_exit);
	}

private:
	viewport& viewport;
	bool early_exit = false;
	bool sigkill = false;
	std::vector<unsigned char> output;

	std::thread thread;
};

#endif //RAYTRACINGWEEKEND_RENDERER_H