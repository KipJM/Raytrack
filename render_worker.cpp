#include "render_worker.h"
#include "viewport.h"

render_worker::render_worker(viewport& vp): viewport_(vp), early_exit(false), sigkill(false)
{
	thread = std::thread(&render_worker::render_loop, this);
}

render_worker::~render_worker()
{
	std::clog<<"WORKER DECONSTRUCT" << '\n';

	early_exit = true;
	sigkill = true;
	thread.join();
}

void render_worker::reset()
{
	early_exit = true;
}

bool render_worker::get_heartbeat() const
{
	return heartbeat;
}


void render_worker::render_loop()
{
	while (!sigkill)
	{
		// std::clog << "thread " << this << ": sigkill: " << sigkill << " early_exit: " << early_exit << '\n';
		heartbeat = true;
		if (render(viewport_.target_scene.camera, viewport_.target_scene.get_render_scene()))
		{
			std::clog << "thread " << this << " render finished!\n";
			viewport_.append_image(output);
		}
		else if (early_exit)
		{
			// reset early exit
			early_exit = false;
		}
	}
	heartbeat = false;
	std::clog << "thread " << this << " quit successfully" << '\n';
}

bool render_worker::render(camera& camera, const hittable& world)
{
	// clear render buffer
	output = std::vector<unsigned char>();
	return camera.render(world, output, early_exit);
}
