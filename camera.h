#ifndef RAYTRACINGWEEKEND_CAMERA_H
#define RAYTRACINGWEEKEND_CAMERA_H

#include "hittable.h"
#include "material.h"

class camera
{
public:
	std::ofstream	output;				   // Path to ppm image file

	double			aspect_ratio	= 1.0; // Width over height
	int				image_width		= 100; // Image width (px)
	double			vfov			= 90;  // Vertical FOV, in degrees


	int				sample_count	= 50;  // Number of rand samples for each pixel (supersampling)
	int				max_bounces		= 10;  // Maximum amount of bounces for a ray
	double			bias			= 0.001; // Fix shadow acne

	void render(const hittable& world)
	{
		initialize();

		output << "P3" << '\n' << image_width << ' ' << image_height << "\n255\n"; //P3: ASCII COLORS, W&H, max value is 255

		for (int j = 0; j < image_height; j++)
		{
			std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;
			for (int i = 0; i < image_width; i++)
			{
				// Per pixel operations
				color pixel_color(0,0,0);

				for (int sample = 0; sample < sample_count; sample++)
				{
					// Per sample operations

					// technically HDR supported
					ray r = get_ray(i, j);
					pixel_color += ray_color(r, max_bounces, world);
				}

				write_color(output, sample_contribution * pixel_color);

			}
		}
		std::clog << "\rDone.                 \n";
		output.close();
	}

private:
	int		image_height;	// Image height (px)
	double	sample_contribution; // the factor of each sample's influence on the pixel
	point3	center;			// Camera Center (3D)
	point3	pixel00_loc;	// Location of screen pixel (0,0) (3D)
	vec3	pixel_delta_u;	// Offset for one pixel to the right (3D)
	vec3	pixel_delta_v;	// Offset for one pixel down (3D)

	void initialize()
	{
		image_height = int(image_width / aspect_ratio);
		image_height = (image_height < 1) ? 1 : image_height;

		sample_contribution = 1.0 / sample_count;

		center = point3(0, 0, 0);

		// Determine viewport dimensions
		auto focal_length = 1.0;
		auto theta = deg_to_rad(vfov);
		auto h = std::tan(theta/2);
		auto viewport_height = 2 * h * focal_length;

		auto viewport_width = viewport_height * (double(image_width) / image_height);

		// Calc viewport UV
		// 3D: right-handed y-up | Screen: x-right y-down
		auto viewport_u = vec3(viewport_width, 0, 0);
		auto viewport_v = vec3(0, -viewport_height, 0);

		// Convert viewport UV to pixel 3D
		pixel_delta_u = viewport_u / image_width;
		pixel_delta_v = viewport_v / image_height;

		// Calc location of upper-left(0,0) pixel
		auto viewport_upper_left = center - vec3(0,0, focal_length) - viewport_u/2 - viewport_v/2;
		pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v); //centered


	}

	vec3 sample_square() const
	{
		// ramdom point in a square [-.5, +.5)
		//	[-+.5-]
		//	|  0  |
		//	[_-.5_]
		return {rand_double() - 0.5, rand_double() - 0.5, 0};
	}

	/// Returns a random ray for pixel (i, j)
	/// a ray from origin to a randomly sampled point around pixel (i,j) on the near plane
	ray get_ray(int i, int j) const
	{
		auto offset = sample_square();
		auto pixel_sample = pixel00_loc
								+ ((i + offset.x()) * pixel_delta_u)
								+ ((j + offset.y()) * pixel_delta_v);

		auto ray_origin = center;
		auto ray_direction = pixel_sample - ray_origin;

		return ray(ray_origin, ray_direction);
	}

	color ray_color(const ray& r, int depth, const hittable& world) const
	{
		if (depth <= 0)
			return {0,0,0}; // TODO: Should be able to modify ambient light from world

		hit_record rec;

		if (world.hit(r, interval(bias, infinity), rec))
		{
			ray scattered;
			color attenuation;

			if (rec.mat->scatter(r, rec, attenuation, scattered))
			{
				return attenuation * ray_color(scattered, depth-1, world);
			}
			return color(0,0,0);
		}

		// sky
		vec3 unit_direction = unit_vector(r.direction());
		auto a = 0.5*(unit_direction.y() + 1.0); // [-1, 1] => [0, 2] => [0, 1]
		return color(1.0, 1.0, 1.0) * (1.0-a) + color(0.5, 0.7, 1.0) * a;
	}
};

#endif //RAYTRACINGWEEKEND_CAMERA_H