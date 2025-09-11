#ifndef RAYTRACINGWEEKEND_CAMERA_H
#define RAYTRACINGWEEKEND_CAMERA_H

#include "hittable.h"
#include "material.h"

class camera
{
public:
	double			aspect_ratio	= -1; // Width over height
	int				image_width		= 100; // Image width (px)
	int				image_height	= -1;

	int				sample_count	= 50;  // Number of rand samples for each pixel (supersampling)
	int				max_bounces		= 10;  // Maximum amount of bounces for a ray
	double			bias			= 0.001; // Fix shadow acne
	color			background		= color(0.70,0.80,1.00); // background color

	double			vfov			= 90;  // Vertical FOV, in degrees
	point3			position		= point3(0,0,0); // camera position
	point3			lookat			= point3(0,0,-1); // look at this point
	vec3			vup				= vec3(0,1,0);    // Camera-relative up direction

	double			defocus_angle	= 0; // Variation angle of rays through each pixel
	double			focus_distance	= 10; // Distance to perfect focus

	void ready()
	{
		initialize();
	}

	bool render(const hittable& world, std::vector<unsigned char>& output)
	{
		bool _ = false;
		return render(world, output, _);
	}

	bool render(const hittable& world, std::vector<unsigned char>& output, bool& early_exit)
	{

		// ppm output disabled
		// output << "P3" << '\n' << image_width << ' ' << image_height << "\n255\n"; //P3: ASCII COLORS, W&H, max value is 255

		for (int j = 0; j < image_height; j++)
		{
			// std::clog << "\rScanlines remaining: " << (image_height - j) << '\n';
			for (int i = 0; i < image_width; i++)
			{
				// Per pixel operations
				color pixel_color(0,0,0);

				for (int sample = 0; sample < sample_count; sample++)
				{
					// Early exit
					if (early_exit)
					{
						std::wclog << "EARLY EXIT!" << '\n';
						return false; // Render cancelled
					}

					// Per sample operations here!

					// technically HDR supported
					ray r = get_ray(i, j);
					pixel_color += ray_color(r, max_bounces, world);
				}

				write_color(output, sample_contribution * pixel_color);

			}
		}
		// std::clog << "\rDone.                 \n";
		return true; // Finished successfully!
	}

private:
	// int		image_height = 0;	// Image height (px)
	double	sample_contribution = 0; // the factor of each sample's influence on the pixel
	point3	center;			// Camera Center (3D)
	point3	pixel00_loc;	// Location of screen pixel (0,0) (3D)
	vec3	pixel_delta_u;	// Offset for one pixel to the right (3D)
	vec3	pixel_delta_v;	// Offset for one pixel down (3D)
	vec3	u, v, w;		// Basis

	vec3 defocus_disk_u;
	vec3 defocus_disk_v;

	void initialize()
	{
		if (image_height < 0 && aspect_ratio > 0)
		{
			// legacy: calculate image height based on ratio
			image_height = int(image_width / aspect_ratio);
		}
		image_height = (image_height < 1) ? 1 : image_height;

		sample_contribution = 1.0 / sample_count;

		center = position;

		// Determine viewport dimensions
		auto theta = deg_to_rad(vfov);
		auto h = std::tan(theta/2);
		auto viewport_height = 2 * h * focus_distance;
		auto viewport_width = viewport_height * (double(image_width) / image_height);

		w = unit_vector((center - lookat));
		u = unit_vector(cross(vup, w));
		v = cross(w, u);

		// Calc viewport UV
		// 3D: right-handed y-up | Screen: x-right y-down
		auto viewport_u = viewport_width * u;
		auto viewport_v = viewport_height * -v;

		// Convert viewport UV to pixel 3D
		pixel_delta_u = viewport_u / image_width;
		pixel_delta_v = viewport_v / image_height;

		// Calc location of upper-left(0,0) pixel
		auto viewport_upper_left = center - (focus_distance * w) - viewport_u/2 - viewport_v/2;
		pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v); //centered

		// defocus
		auto defocus_radius = focus_distance * std::tan(deg_to_rad(defocus_angle / 2));
		defocus_disk_u = u * defocus_radius;
		defocus_disk_v = v * defocus_radius;

	}

	vec3 sample_square() const
	{
		// ramdom point in a square [-.5, +.5)
		//	[-+.5-]
		//	|  0  |
		//	[_-.5_]
		return {rand_double() - 0.5, rand_double() - 0.5, 0};
	}

	point3 defocus_disk_sample() const
	{
		auto p = rand_unit_disk_vector();
		return center + (p.x() * defocus_disk_u) + (p.y() * defocus_disk_v);
	}

	/// Returns a random ray for pixel (i, j)
	/// a ray from defocus disk to a randomly sampled point around pixel (i,j) on the near plane
	ray get_ray(int i, int j) const
	{
		auto offset = sample_square();
		auto pixel_sample = pixel00_loc
								+ ((i + offset.x()) * pixel_delta_u)
								+ ((j + offset.y()) * pixel_delta_v);

		auto ray_origin = (defocus_angle <= 0) ? center : defocus_disk_sample();
		auto ray_direction = pixel_sample - ray_origin;
		auto ray_time = rand_double();

		return ray(ray_origin, ray_direction, ray_time);
	}

	color ray_color(const ray& r, int depth, const hittable& world) const
	{
		if (depth <= 0)
			return {0,0,0}; // TODO: Should be able to modify ambient light from world

		hit_record rec;

		if (!world.hit(r, interval(bias, infinity), rec))
			return background;


		ray scattered;
		color attenuation;
		color color_from_emission = rec.mat->emitted(rec.u, rec.v, rec.p);

		if (!rec.mat->scatter(r, rec, attenuation, scattered))
			return color_from_emission;


		color color_from_scatter = attenuation * ray_color(scattered, depth - 1, world);
		return color_from_emission + color_from_scatter;
	}
};

#endif //RAYTRACINGWEEKEND_CAMERA_H