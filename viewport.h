#ifndef RAYTRACINGWEEKEND_VIEWPORT_H
#define RAYTRACINGWEEKEND_VIEWPORT_H
#include <queue>
#include "include/glad/glad.h" // because CLion is fucking stupid

#include "render_worker.h"
#include "scene.h"

class viewport
{
public:
	scene& target_scene;
	std::vector<int> density_map;  // amount of samples per pixel /// PLEASE DO NOT EDIT OH MY GOD WHY NO LAMBDAS

	viewport() = delete;

	viewport(scene& scene_, int resolution_width, int resolution_height, int workers_count) : target_scene(scene_)
	{
		// Set basic configs
		max_bounces = 20;
		bias = 0.001;
		sample_count = 1;
		min_samples = 30;
		basic_ratio = 0.1;
		fill_ratio = 0.7;
		init_new_camera();

		// init gl texture
		glGenTextures(1, &texture_id);
		glBindTexture(GL_TEXTURE_2D, texture_id);

		float border_color[] = { 0.0f, 0.0f, 0.0f, 1.0f};
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_color);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);


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

	bool set_worker_count(int count, bool override = false)
	{
		if (count < 1 && !override)
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

	camera& get_camera()
	{
		return target_scene.camera;
	}

	int get_width() const
	{
		return target_scene.camera.image_width;
	}

	int get_height() const
	{
		return target_scene.camera.image_height;
	}

	[[nodiscard]] int get_current_sample_count() const
	{
		return current_samples;
	}

	[[nodiscard]] bool is_waiting() const
	{
		if (dirty) return true;
		if (get_current_sample_count() == 0) return true;
		return false;
	}

	bool is_dirty()
	{
		return dirty;
	}

	bool mark_dirty()
	{
		auto was_dirty = dirty;
		dirty = true;
		return was_dirty;
	}

	int temp_workers_count;
	bool mark_scene_dirty()
	{
		if (target_scene.is_dirty())
			return true;

		temp_workers_count = get_workers_count();
		set_worker_count(0, true);
		return target_scene.mark_dirty();
	}

	void append_image(std::vector<float>& image)
	{
		if (dirty)
			return;

		if (backlog.size() > 500) // if resolution is too small, samples are submitted faster than update cycles and memory breaks
			return;

		if (image.size() != current_tex.size())
		{
			std::clog<<":O";
			return;
		}

		backlog.push(std::move(image));
		current_samples += target_scene.camera.sample_count;
	}

	void update()
	{
		if (dirty || target_scene.is_dirty())
		{
			if (target_scene.is_dirty())
			{
				target_scene.update();
				set_worker_count(temp_workers_count);
			}
			// Stop everything! Reset renderers
			// Don't clear the texture, prevent flickering
			reset();
			return;
		}

		if (!backlog.empty())
		{
			std::clog<<"backlog: " << backlog.size()<<"\n";


			// merge textures
			auto &tex = backlog.front();

			if (tex.size() == current_tex.size())
			{
				for (int i = 0; i < current_tex.size(); i++)
				{
					// Mix previous textures and new texture, normalization will be done later
					// -1 means pixel is skipped, so no contribution
					if (tex[i] >= 0)
					{
						current_tex[i] += tex[i];
						density_map[i] += 1;
					}
				}

				// normalization; convert to sdr

				std::vector<float> out_gamma(current_tex.size());

				for (int i = 0; i < current_tex.size(); i++)
				{
					if (density_map[i] == 0 )
						out_gamma[i] = 0.0f;
					else
					{
						auto compensation = 1.0f / static_cast<float>(density_map[i]);
						out_gamma[i] = linear_to_gamma(current_tex[i] * compensation);
					}
				}

				backlog.pop();
				index++;


				// update screen!

				// OpenGL: sub texture
				glBindTexture(GL_TEXTURE_2D, texture_id);
				glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,
					get_width(), get_height(),
					GL_RGB, GL_FLOAT, out_gamma.data());
			} else
			{
				std::clog<<":(";
				backlog.pop(); // skip
				// backlog = std::queue<std::vector<float>>(); // clear backlog to fix weirdness
			}
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
		if (resolution_width <= 10 || resolution_height <= 10)
			return;


		mark_dirty();
		target_scene.camera.image_width = resolution_width;
		target_scene.camera.image_height = resolution_height;

		reset();

		// OpenGL: Change texture (resolution change)
		glBindTexture(GL_TEXTURE_2D, texture_id);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, resolution_width, resolution_height, 0,
			GL_RGB, GL_FLOAT, current_tex.data());

	}

	void reset()
	{
		// clear backlog
		std::queue<std::vector<float>> empty;
		std::swap( backlog, empty );

		// update resolution, clear data
		current_tex = std::vector<float>(get_width() * get_height() * channels_per_pixel);
		density_map = std::vector<int>(get_width() * get_height() * channels_per_pixel);

		target_scene.camera.ready();

		// reset workers
		for (auto &worker : workers)
		{
			worker->reset();
		}

		index = 0;
		dirty = false;
		current_samples = 0;
	}

	void init_new_camera()
	{
		camera& cam = get_camera();
		cam.max_bounces = max_bounces;
		cam.bias = bias;
		cam.sample_count = sample_count;
		cam.min_samples = min_samples;
		cam.basic_ratio = basic_ratio;
		cam.fill_ratio = fill_ratio;
		// cam.ready();
		mark_dirty();
	}

private:
	int current_samples = 0;
	int index = 0;
	bool dirty = false; // Viewport resolution changed
	int channels_per_pixel = 3;
	std::queue<std::vector<float>> backlog;
	std::vector<float> current_tex; // color data

	std::vector<std::unique_ptr<render_worker>> workers;

	// opengl
	GLuint texture_id;

private:
	// persistent settings
	int max_bounces;
	double bias;
	int sample_count;
	int min_samples;
	double basic_ratio;
	double fill_ratio;

public:
	[[nodiscard]] int get_max_bounces() const
	{
		return max_bounces;
	}

	void set_max_bounces(int max_bounces)
	{
		if (max_bounces < 0) max_bounces = 0;
		this->max_bounces = max_bounces;
		get_camera().max_bounces = max_bounces;
		mark_dirty();
	}

	[[nodiscard]] double get_bias() const
	{
		return bias;
	}

	void set_bias(double bias)
	{
		if (bias < 0) bias = 0.001;
		this->bias = bias;
		get_camera().bias = bias;
		mark_dirty();
	}

	[[nodiscard]] int get_sample_count() const
	{
		return sample_count;
	}

	void set_sample_count(int sample_count)
	{
		if (sample_count < 1) sample_count = 1;
		this->sample_count = sample_count;
		get_camera().sample_count = sample_count;
		mark_dirty();
	}

	[[nodiscard]] int get_min_samples() const
	{
		return min_samples;
	}

	void set_min_samples(int min_samples)
	{
		if (min_samples < 0) min_samples = 0;
		this->min_samples = min_samples;
		get_camera().min_samples = min_samples;
		mark_dirty();
	}

	[[nodiscard]] double get_basic_ratio() const
	{
		return basic_ratio;
	}

	void set_basic_ratio(double basic_ratio)
	{
		if (basic_ratio <= 0) basic_ratio = 0.001;
		if (basic_ratio > 1) basic_ratio = 1;
		this->basic_ratio = basic_ratio;
		get_camera().basic_ratio = basic_ratio;
		mark_dirty();
	}

	[[nodiscard]] double get_fill_ratio() const
	{
		return fill_ratio;
	}

	void set_fill_ratio(double fill_ratio)
	{
		if (fill_ratio <= 0) fill_ratio = 0.001;
		if (fill_ratio > 1) fill_ratio = 1;
		this->fill_ratio = fill_ratio;
		get_camera().fill_ratio = fill_ratio;
		mark_dirty();
	}
};

#endif //RAYTRACINGWEEKEND_VIEWPORT_H