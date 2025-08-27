#ifndef VEC3_H
#define VEC3_H

class vec3
{
public:
	double e[3];

	vec3() : e{0, 0, 0}	{};
	vec3(double e0, double e1, double e2) : e{e0, e1, e2} {};

	double x() const {return e[0];};
	double y() const {return e[1];};
	double z() const {return e[2];};

	vec3 operator -() const { return vec3(-e[0], -e[1], -e[2]); };
	double operator [](int i) const { return e[i]; };
	double& operator [](int i ) { return e[i]; };

	vec3& operator +=(const vec3& v)
	{
		e[0] += v.e[0];
		e[1] += v.e[1];
		e[2] += v.e[2];
		return *this;
	}

	vec3& operator *=(double t)
	{
		e[0] *= t;
		e[1] *= t;
		e[2] *= t;
		return *this;
	}

	vec3& operator /=(double t)
	{
		return *this *= 1/t;
	}

	double length() const
	{
		return std::sqrt(length_squared());
	}

	double length_squared() const
	{
		return e[0]*e[0] + e[1]*e[1] + e[2]*e[2];
	}

	bool near_zero() const
	{
		auto s = 1e-8;
		return (std::fabs(e[0]) < s) && (std::fabs(e[1]) < s) && (std::fabs(e[2]) < s);
	}

	// Static

	static vec3 random()
	{
		return {rand_double(), rand_double(), rand_double()};
	}

	static vec3 random(double min, double max)
	{
		// More random to do this than random * min max I think?
		return {rand_double(min, max), rand_double(min, max), rand_double(min, max)};
	}
};

// point3 as alias for vec3
using point3 = vec3;

//Vector Utils
inline std::ostream& operator << (std::ostream& out, const vec3& v) // plumbing hell
{
	return out << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2];
}

inline vec3 operator + (const vec3& u, const vec3& v)
{
	return vec3(u.e[0] + v.e[0], u.e[1] + v.e[1], u.e[2] + v.e[2]);
}

inline vec3 operator - (const vec3& u, const vec3& v)
{
	return vec3(u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2]);
}

inline vec3 operator * (const vec3& u, const vec3& v)
{
	return vec3(u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2]);
}

inline vec3 operator * (double t, const vec3& v)
{
	return vec3(t*v.e[0], t*v.e[1], t*v.e[2]);
}

inline vec3 operator * (const vec3& v, double t)
{
	return t * v;
}

inline vec3 operator / (const vec3& v, double t)
{
	return (1/t) * v;
}

inline double dot(const vec3& u, const vec3& v)
{
	return u.e[0] * v.e[0]
		+ u.e[1] * v.e[1]
		+ u.e[2] * v.e[2];
}

inline vec3 cross(const vec3& u, const vec3& v)
{
	return vec3(u.e[1] * v.e[2] - u.e[2] * v.e[1],
				u.e[2] * v.e[0] - u.e[0] * v.e[2],
				u.e[0] * v.e[1] - u.e[1] * v.e[0]);
}

inline vec3 unit_vector(const vec3& v)
{
	return v / v.length();
}

inline vec3 rand_unit_vector()
{
	// brute force a solution
	while (true)
	{
		auto p = vec3::random(-1,1);
		auto lensq = p.length_squared();
		// only accept vectors in sphere and non-zero for uniform distribution
		if (lensq <= 1 && lensq > 1e-160)
		{
			return p / sqrt(lensq); // Skips a square this way
		}
	}
}

inline vec3 rand_hemisphere_vector(const vec3& normal)
{
	vec3 on_unit_sphere = rand_unit_vector();
	if (dot(on_unit_sphere, normal) > 0.0) // Same hemisphere
		return on_unit_sphere;
	else
		return -on_unit_sphere; // Inverted, now on same hemisphere
}

inline vec3 reflect(const vec3& v, const vec3& n) // normal is normalized
{
	return v - 2 * dot(v, n) * n;
}

inline vec3 refract(const vec3& uv, const vec3& n, double etai_over_etat)
{
	auto cos_theta = std::fmin(dot(-uv, n), 1.0); // yay no trig functions
	vec3 r_out_perp = etai_over_etat * (uv + cos_theta*n);
	vec3 r_out_parallel = -std::sqrt(std::fabs(1.0 - r_out_perp.length_squared())) * n;
	return r_out_perp + r_out_parallel;
}

#endif