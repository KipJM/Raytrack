#ifndef RAYTRACINGWEEKEND_VIEWPORT_H
#define RAYTRACINGWEEKEND_VIEWPORT_H
#include <memory>
#include <queue>

#include "render_worker.h"
#include "scene.h"
#include "include/glad/glad.h" // because CLion is fucking stupid



class viewport
{
public:
	scene target_scene;
	std::vector<int> density_map;  // amount of samples per pixel /// PLEASE DO NOT EDIT OH MY GOD WHY NO LAMBDAS

	viewport() = delete;

	viewport(scene _scene, int resolution_width, int resolution_height, int workers_count);

	unsigned int get_texture_id() const;

	[[nodiscard]] int get_workers_count() const;

	bool set_worker_count(int count, bool override = false);

	camera& get_camera()
	{
		return target_scene.s_camera;
	}

	int get_width() const
	{
		return target_scene.s_camera.image_width;
	}

	int get_height() const
	{
		return target_scene.s_camera.image_height;
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
	bool mark_scene_dirty();

	void append_image(std::vector<float>& image);

	void update();

	void set_resolution(int resolution_width, int resolution_height);

	void reset();

	void init_new_camera();

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
	int dark_samples; // how much the worker prefer dark pixels lol

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

	[[nodiscard]] int get_dark_samples() const
	{
		return dark_samples;
	}

	void set_dark_samples(int _dark_samples)
	{
		if (_dark_samples < 0) _dark_samples = 0;
		this->dark_samples = _dark_samples;
		get_camera().dark_samples = _dark_samples;
		mark_dirty();
	}
};

#endif //RAYTRACINGWEEKEND_VIEWPORT_H