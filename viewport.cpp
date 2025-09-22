#include "viewport.h"

viewport::viewport(scene scene_, int resolution_width, int resolution_height, int workers_count): target_scene(std::move(scene_))
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

unsigned int viewport::get_texture_id() const
{ return texture_id; }

int viewport::get_workers_count() const
{
	return static_cast<int>(workers.size());
}

bool viewport::set_worker_count(int count, bool override)
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

bool viewport::mark_scene_dirty()
{
	if (target_scene.is_dirty())
		return true;

	temp_workers_count = get_workers_count();
	set_worker_count(0, true);
	return target_scene.mark_dirty();
}

void viewport::append_image(std::vector<float>& image)
{
	if (dirty)
		return;

	if (backlog.size() > 100) // if resolution is too small, samples are submitted faster than update cycles, then memory grows too large
		return;

	if (image.size() != current_tex.size())
	{
		std::clog<<":O";
		return;
	}

	backlog.push(std::move(image));
	current_samples += target_scene.camera.sample_count;
}

void viewport::update()
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
			if (!backlog.empty())
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

void viewport::set_resolution(int resolution_width, int resolution_height)
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

void viewport::reset()
{
	// clear backlog
	std::queue<std::vector<float>> empty;
	std::swap( backlog, empty );

	// update resolution, clear data
	target_scene.camera.ready();

	current_tex = std::vector<float>(get_width() * get_height() * channels_per_pixel);
	density_map = std::vector<int>(get_width() * get_height() * channels_per_pixel);

	// reset workers
	for (auto &worker : workers)
	{
		worker->reset();
	}

	index = 0;
	dirty = false;
	current_samples = 0;
}

void viewport::init_new_camera()
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
