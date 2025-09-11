#ifndef RAYTRACINGWEEKEND_VIEWPORT_H
#define RAYTRACINGWEEKEND_VIEWPORT_H
#include <queue>

#include "render_worker.h"
#include "scene.h"

class viewport
{
public:
	scene& target_scene;

	viewport(scene& scene_, int resolution_width, int resolution_height, int workers_count) :
		target_scene(scene_),
		width(resolution_width), height(resolution_height)
	{

		set_resolution(resolution_width, resolution_height);
		reset();

		// Generate workers. They will automatically get to work.
		for (int i = 0; i < workers_count; i++)
		{
			workers.push_back(std::make_unique<render_worker>(*this));
		}

		// todo: create texture!
		std::clog << "constructor exit\n";
	}

	[[nodiscard]] int get_workers_count() const
	{
		return static_cast<int>(workers.size());
	}

	bool set_worker_count(int count)
	{
		if (count < 1)
			return false;

		if (count > workers.size())
		{
			auto target = count - workers.size();
			for (int i = 0; i < target; i++)
			{
				workers.push_back(std::make_unique<render_worker>(*this));
			}
			return true;
		}
		else if (count < workers.size())
		{
			auto target = workers.size() - count;
			for (int i = 0; i < target; i++)
			{
				workers.pop_back();
			}
			return true;
		}
		else
		{
			return false;
		}

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
		if (dirty || target_scene.is_dirty())
		{
			if (target_scene.is_dirty())
			{
				target_scene.update();
			}
			// Stop everything! Reset renderers
			// Don't clear the texture, prevent flickering
			reset();
		}

		if (!backlog.empty())
		{
			// merge textures
			auto tex = backlog.front();
			for (int i = 0; i < width * height * bytes_per_pixel; i++)
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

		for (int i = 0; i < workers.size(); i++)
		{
			if (!workers[i]->get_heartbeat())
			{
				std::clog << "worker" << i << " no heartbeat!\n";
			} else
			{
				// std::clog << "worker " << i << " working\n";
			}
		}
	}

	void set_resolution(int resolution_width, int resolution_height)
	{
		mark_dirty();
		width = resolution_width;
		height = resolution_height;

		target_scene.camera.image_width = width;
		target_scene.camera.image_height = height;

		// update resolution
		current_tex = std::vector<unsigned char>(width * height * bytes_per_pixel);
		// TODO: opengl change texture
		reset();
	}

	void reset()
	{
		// do not clear current tex to prevent excessive memory juggling
		// clear backlog
		std::queue<std::vector<unsigned char>> empty;
		std::swap( backlog, empty );

		target_scene.camera.ready();

		// reset workers
		for (auto &worker : workers)
		{
			worker.reset();
		}

		index = 0;
		dirty = false;
	}

private:
	int width;
	int height;

	int index = 0;
	bool dirty = false; // Viewport resolution changed
	int bytes_per_pixel = 3;
	std::queue<std::vector<unsigned char>> backlog;
	std::vector<unsigned char> current_tex;

	std::vector<std::unique_ptr<render_worker>> workers;
};

#endif //RAYTRACINGWEEKEND_VIEWPORT_H