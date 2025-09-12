#ifndef RAYTRACINGWEEKEND_RENDERER_H
#define RAYTRACINGWEEKEND_RENDERER_H
#include <thread>

#include "camera.h"
#include "hittable_list.h"

class viewport;

class render_worker
{
public:
	bool get_heartbeat() const;

	explicit render_worker(viewport& vp);

	~render_worker();

	void reset();

	void render_loop();

	bool render(camera& camera, const hittable& world);

private:
	viewport& viewport_;
	bool early_exit = false;
	bool sigkill = false;
	std::vector<float> output;

	std::thread thread;

	bool heartbeat = false;
};



#endif //RAYTRACINGWEEKEND_RENDERER_H
