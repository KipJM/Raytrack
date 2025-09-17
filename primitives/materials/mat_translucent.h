#ifndef RAYTRACINGWEEKEND_TRANSLUCENT_H
#define RAYTRACINGWEEKEND_TRANSLUCENT_H

class mat_translucent : public material
{
public:
	mat_translucent(double refraction_index) : refraction_index(refraction_index) {}

	bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override
	{
		attenuation = color(1, 1, 1); //TODO
		double ri = rec.front_face ? (1.0/refraction_index) : refraction_index; // Air refraction index is basically 1

		vec3 unit_direction = unit_vector(r_in.direction());

		double cos_theta = std::fmin(dot(-unit_direction, rec.normal), 1.0);
		double sin_theta = std::sqrt(1.0 - cos_theta * cos_theta);

		bool cannot_refract = ri * sin_theta > 1.0;

		vec3 direction;
		if (cannot_refract || reflectance(cos_theta, ri) > rand_double())
		{
			direction = reflect(unit_direction, rec.normal);
		}else
		{
			direction = refract(unit_direction, rec.normal, ri);
		}

		scattered = ray(rec.p, direction, r_in.time());
		return true;
	}

	material_type get_type() const override {return material_type::Translucent;}
private:
	double refraction_index;

	static double reflectance(double cosine, double refraction_index)
	{
		// schlick approximation
		auto r0 = (1 - refraction_index) / (1 + refraction_index);
		r0 *= r0; // square
		return r0 + (1-r0)*std::pow((1-cosine), 5);
	}
};

#endif //RAYTRACINGWEEKEND_TRANSLUCENT_H