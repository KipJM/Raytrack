#ifndef RAYTRACINGWEEKEND_VIEWPORT_H
#define RAYTRACINGWEEKEND_VIEWPORT_H
#include <queue>

#include "render_worker.h"
#include "scene.h"
#include "glad/glad.h"

class viewport
{
public:
	scene& target_scene;

	viewport() = delete;

	viewport(scene& scene_, int resolution_width, int resolution_height, int workers_count) : target_scene(scene_)
	{
		// init gl texture
		glGenTextures(1, &texture_id);
		glBindTexture(GL_TEXTURE_2D, texture_id);

		float border_color[] = { 0.0f, 0.0f, 0.0f, 1.0f};
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_color);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


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

	unsigned int get_texture_id() const { return texture_id; }

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

	int get_width() const
	{
		return target_scene.camera.image_width;
	}

	int get_height() const
	{
		return target_scene.camera.image_height;
	}

	bool mark_dirty()
	{
		auto was_dirty = dirty;
		dirty = true;
		return was_dirty;
	}

	void append_image(std::vector<float>& image)
	{
		backlog.push(std::move(image));
		current_samples += target_scene.camera.sample_count;
	}

	[[nodiscard]] int get_current_sample_count() const
	{
		return current_samples;
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
			std::clog<<"backlog: " << backlog.size()<<"\n";
			// merge textures
			auto &tex = backlog.front();

			// float intensity = 0;
			auto f_index = static_cast<float>(index);
			for (int i = 0; i < current_tex.size(); i++)
			{
				// Mix previous textures and new texture with same ratio
				// prev: (n)/(n+1) | current: (1)/(n+1)
				current_tex[i] = f_index / (f_index + 1.0f) * current_tex[i] + tex[i] * (1.0f / (f_index+1.0f));
				// intensity += current_tex[i];
			}

			// std::clog << "intensity: " << intensity << '\n';

			// convert to sdr
			std::vector<float> out_gamma(current_tex.size());
			for (int i = 0; i < current_tex.size(); i++)
			{
				out_gamma[i] = linear_to_gamma(current_tex[i]);
			}

			backlog.pop();
			index++;


			// update screen!

			// OpenGL: sub texture
			glBindTexture(GL_TEXTURE_2D, texture_id);
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,
				get_width(), get_height(),
				GL_RGB, GL_FLOAT, out_gamma.data());
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
		target_scene.camera.image_width = resolution_width;
		target_scene.camera.image_height = resolution_height;

		// update resolution
		current_tex = std::vector<float>(get_width() * get_height() * channels_per_pixel);

		// OpenGL: Change texture (resolution change)
		glBindTexture(GL_TEXTURE_2D, texture_id);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, resolution_width, resolution_height, 0,
			GL_RGB, GL_FLOAT, current_tex.data());

		reset();
	}

	void reset()
	{
		// do not clear current tex to prevent excessive memory juggling
		// clear backlog
		std::queue<std::vector<float>> empty;
		std::swap( backlog, empty );

		target_scene.camera.ready();

		// reset workers
		for (auto &worker : workers)
		{
			worker.reset();
		}

		index = 0;
		dirty = false;
		current_samples = 0;
	}

private:
	int current_samples = 0;
	int index = 0;
	bool dirty = false; // Viewport resolution changed
	int channels_per_pixel = 3;
	std::queue<std::vector<float>> backlog;
	std::vector<float> current_tex;

	std::vector<std::unique_ptr<render_worker>> workers;

	// opengl
	GLuint texture_id;
};

#endif //RAYTRACINGWEEKEND_VIEWPORT_H