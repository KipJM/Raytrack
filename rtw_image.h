#ifndef RAYTRACINGWEEKEND_RTW_STB_IMAGE_H
#define RAYTRACINGWEEKEND_RTW_STB_IMAGE_H

#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG
#include "external/stb_image.h"

#include <cstdlib>
#include <iostream>


class rtw_image
{
public:
	rtw_image() {}

	rtw_image(const char* image_filename)
	{
		auto name = std::string(image_filename);
		if (load(name)) return;

		std::cerr << "ERR: Image '" << name << "' could not be loaded.\n";
	}

	~rtw_image()
	{
		delete [] bdata;
		STBI_FREE(fdata);
	}

	/// Loads linear (gamma=1) *SDR* image data from the file
	/// Returns true if load succeeded
	bool load(const std::string& filename)
	{
		auto n = bytes_per_pixel;
		fdata = stbi_loadf(filename.c_str(), &image_width, &image_height, &n, bytes_per_pixel);
		if (fdata == nullptr) return false;
		bytes_per_line = image_width * bytes_per_pixel;
		convert_to_bytes();
		return true;
	}

	int width() const { return (fdata == nullptr) ? 0 : image_width; }
	int height() const { return (fdata == nullptr) ? 0 : image_height; }

	const unsigned char* pixel_data(int x, int y) const
	{
		static unsigned char error_color[] = {255, 0, 255};
		if (bdata == nullptr) return error_color;

		x = clamp(x, 0, image_width);
		y = clamp(y, 0, image_height);

		return bdata + bytes_per_line * y + x * bytes_per_pixel; // pointers hell (cont.)

	}

private:
	const int	bytes_per_pixel = 3;
	int			bytes_per_line = 0;
	float		*fdata = nullptr;		// Linear floating point image data
	unsigned char *bdata = nullptr;		// Linear 8-bit image data (C: no distinction between types except memory allocation)
	int			image_width = 0;
	int			image_height = 0;


	/// Convert the linear float image data to bytes, stored to bdata
	void convert_to_bytes()
	{
		int total_bytes = image_width * image_height * bytes_per_pixel;
		bdata = new unsigned char[total_bytes];

		// convert float [0.0, 1.0] to unsigned byte [0, 255] for each pixel
		auto *bptr = bdata; // omg pointers fucking sucks
		auto *fptr = fdata;
		for (auto i = 0; i < total_bytes; i++, fptr++, bptr++)
		{
			*bptr = float_to_byte(*fptr);
		}
	}

	static unsigned char float_to_byte(float value)
	{
		if (value <= 0.0)
			return 0;
		if (value >= 1.0)
			return 255;

		return static_cast<unsigned char>(256.0 * value);
	}


	/// Returns x clamped to [low, high)
	static int clamp(int x, int low, int high)
	{
		if (x < low) return low;
		if (x < high) return x;
		return high - 1;
	}
};

#endif //RAYTRACINGWEEKEND_RTW_STB_IMAGE_H