#ifndef RAYTRACINGWEEKEND_VIEWPORT_H
#define RAYTRACINGWEEKEND_VIEWPORT_H
#include <queue>
#include <thread>

#include "render_worker.h"
#include "scene.h"

class viewport
{
public:
	scene& scene;

	viewport(int resolution_width, int resolution_height, int workers_count) : width(resolution_width), height(resolution_height)
	{
		set_resolution(resolution_width, resolution_height);
		index = 0;
		dirty = true;

		// Generate workers. They will automatically get to work.
		for (int i = 0; i < workers_count; i++)
		{
			workers.push_back(render_worker(this));
		}

		// todo: create texture!
	}

	bool mark_dirty()
	{
		auto was_dirty = dirty;
		dirty = true;
		return was_dirty;
	}

	void append_image(const std::vector<unsigned char>& image)
	{
		backlog.push(image);
	}

	void update()
	{

		if (dirty || scene.is_dirty())
		{
			if (scene.is_dirty())
			{
				scene.update();
			}
			// Stop everything! Reset renderers
			// Don't clear the texture, prevent flickering
			reset();
		}

		if (!backlog.empty())
		{
			// merge textures
			auto tex = backlog.front();
			for (int i = 0; i < current_tex.size(); i++)
			{
				// Mix previous textures and new texture with same ratio
				// prev: (n)/(n+1) | current: (1)/(n+1)
				current_tex[i] = current_tex[i] * (index) / (index+1) + tex[i] * 1 / (index+1);
			}

			backlog.pop();
			index++;

			// update screen!

			// TODO: opengl sub texture!
		}
	}

	void set_resolution(int resolution_width, int resolution_height)
	{
		mark_dirty();
		width = resolution_width;
		height = resolution_height;

		// update resolution
		current_tex = std::vector<unsigned char>(width * height * bytes_per_pixel);
		// TODO: opengl change texture
	}

	void reset()
	{
		// do not clear current tex to prevent excessive memory juggling
		// clear backlog
		std::queue<std::vector<unsigned char>> empty;
		std::swap( backlog, empty );

		// reset workers
		for (auto &worker : workers)
		{
			worker.reset();
		}

		index = 0;
	}

private:
	int width;
	int height;

	int index = 0;
	bool dirty = false; // Viewport resolution changed
	int bytes_per_pixel = 3;
	std::queue<std::vector<unsigned char>> backlog;
	std::vector<unsigned char> current_tex;

	std::vector<render_worker> workers;
};

#endif //RAYTRACINGWEEKEND_VIEWPORT_H