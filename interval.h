//
// Created by mayua on 2025/08/24.
//

#ifndef RAYTRACINGWEEKEND_INTERVAL_H
#define RAYTRACINGWEEKEND_INTERVAL_H

class interval
{
public:
	double min, max;

	interval() : min(+infinity), max(-infinity) {} // Default: empty interval
	interval(double min, double max) : min(min), max(max) {}

	double size() const
	{
		return max - min;
	}

	bool contains(double x) const
	{
		return min <= x && x <= max;
	}

	bool surrounds(double x) const
	{
		return min < x && x < max;
	}


	double clamp(double x) const
	{
		if (x < min) return min;
		if (x > max) return max;
		return x;
	}

	static const interval empty, universe;

};

const interval interval::empty		= interval(+infinity, -infinity);
const interval interval::universe	= interval(-infinity, +infinity);

#endif //RAYTRACINGWEEKEND_INTERVAL_H