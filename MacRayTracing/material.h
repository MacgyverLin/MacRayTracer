#ifndef _MATERIAL_h_
#define _MATERIAL_h_

#include "ray3.h"
#include "trace_record.h"

class texture
{
public:
	texture()
	{
	}

	virtual vec3 sample(float u, float v, const vec3& p) const = 0;
};

class const_texture : public texture
{
public:
	const_texture(const vec3& color_)
	: color(color_)
	{
	}

	virtual vec3 sample(float u, float v, const vec3& p) const
	{
		return color;
	}

	vec3 color;
};

class perlin
{
public:
	perlin()
	{
		if (!inited)
		{
			perlin_generate();
			perlin_generate_prem(perm_x);
			perlin_generate_prem(perm_y);
			perlin_generate_prem(perm_z);

			inited = true;
		}
	}

	float sample(const vec3& p) const
	{
		float u = p.x() - floor(p.x());
		float v = p.y() - floor(p.y());
		float w = p.z() - floor(p.z());

		int i = int(4 * p.x()) & 255;
		int j = int(4 * p.y()) & 255;
		int k = int(4 * p.z()) & 255;

		return ranfloat[perm_x[i] ^ perm_y[j] ^ perm_z[k]];
	}

	static void perlin_generate()
	{
		for (int i = 0; i < 256; i++)
		{
			ranfloat[i] = random();
		}
	}

	static void permute(int* p, int n)
	{
		for (int i = n - 1; i > 0; i--)
		{
			int target = int(random() * (i + 1));
			int tmp = p[i];
			p[i] = p[target];
			p[target] = tmp;
		}
	}

	static void perlin_generate_prem(int *p)
	{
		for (int i = 0; i < 256; i++)
			p[i] = i;

		permute(p, 256);
	}

	static bool inited;
	static float ranfloat[256];
	static int perm_x[256];
	static int perm_y[256];
	static int perm_z[256];
};

bool perlin::inited = false;
float perlin::ranfloat[256];
int perlin::perm_x[256];
int perlin::perm_y[256];
int perlin::perm_z[256];

class perlin_noise_texture : public texture
{
public:
	perlin_noise_texture(const vec3& color_ = vec3(1, 1, 1))
	: color(color_)
	{
	}

	virtual vec3 sample(float u, float v, const vec3& p) const
	{
		return vec3(1, 1, 1) * noise.sample(p);
	}

	vec3 color;
	perlin noise;
};

class checker_texture : public texture
{
public:
	checker_texture(shared_ptr<texture> texture0_, shared_ptr<texture> texture1_, const vec3& size_ = vec3(1, 1, 1))
		: texture0(texture0_)
		, texture1(texture1_)
		, size(size_)
	{
	}

	virtual vec3 sample(float u, float v, const vec3& p) const
	{
		float a = sin(p.x() * 3.14 / size.x()) * sin(p.y() * 3.14 / size.y()) * sin(p.z() * 3.14 / size.z());
		if (a > 0)
		{
			return texture0->sample(u, v, p);
		}
		else
		{
			return texture1->sample(u, v, p);
		}
	}

	shared_ptr<texture> texture0;
	shared_ptr<texture> texture1;
	vec3 size;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class material
{
public:
	material()
	{
	}

	virtual bool scatter(const ray3& in, const trace_record& record, vec3& atteunation, ray3& scattered) const = 0;
};

class lambertian : public material
{
public:
	lambertian(shared_ptr<texture> albedo)
	{
		this->albedo = albedo;
	}

	virtual bool scatter(const ray3& in, const trace_record& record, vec3& atteunation, ray3& scattered) const
	{
		vec3 target = record.position + record.normal + random_in_unit_sphere();
		
		scattered = ray3(record.position, target - record.position, in.time());
		
		atteunation = albedo->sample(0, 0, record.position);

		return true;
	}

	shared_ptr<texture> albedo;
};

class metal : public material
{
public:
	metal(const vec3& albedo, float fuzz = 0.0)
	{
		this->albedo = albedo;
		this->fuzz = fuzz;
	}

	vec3 reflect(const vec3& v, const vec3& n) const
	{
		return v - 2 * dot(v, n) * n;
	}

	virtual bool scatter(const ray3& in, const trace_record& record, vec3& atteunation, ray3& scattered) const
	{
		vec3 reflected = reflect(unit_vector(in.direction()), record.normal);
		scattered = ray3(record.position, reflected + fuzz*random_in_unit_sphere(), in.time());
		atteunation = albedo;
		
		return dot(scattered.direction(), record.normal)>0;
	}

	vec3 albedo;
	float fuzz;
};

class dielectric : public material
{
public:
	dielectric(vec3 albedo, float ior)
	{
		this->albedo = albedo;
		this->ior = ior;
	}

	float schlick(float cosine, float ior) const
	{
		auto r0 = (1 - ior) / (1 + ior);
		r0 = r0 * r0;
		return r0 + (1 - r0) * pow((1 - cosine), 5);
	}

	vec3 reflect(const vec3& v, const vec3& n) const
	{
		return v - 2 * dot(v, n) * n;
	}

	bool refract(const vec3& v, const vec3& n, float ni_over_nt, vec3& refracted) const
	{
		vec3 uv = unit_vector(v);
		float dt = dot(uv, n);
		float discriminant = 1.0 - ni_over_nt * ni_over_nt * (1.0 - dt * dt);
		if (discriminant > 0)
		{
			refracted = ni_over_nt * (uv - n * dt) - n * sqrt(discriminant);
			return true;
		}
		else
			return false;
	}

	virtual bool scatter2(const ray3& in, const trace_record& record, vec3& attenuation, ray3& scattered) const
	{
		vec3 outward_normal;
		vec3 reflected = reflect(in.direction(), record.normal);
		float ni_over_nt;
		vec3 refracted;
		float cosine;
		float reflect_prob;
		attenuation = albedo;

		if (dot(in.direction(), record.normal) > 0)
		{
			outward_normal = -record.normal;
			ni_over_nt = ior;
			
			cosine = ior * dot(in.direction(), record.normal) / in.direction().length();
		}
		else
		{
			outward_normal = record.normal;
			ni_over_nt = 1.0 / ior;

			cosine = -dot(in.direction(), record.normal) / in.direction().length();
		}

		if (refract(in.direction(), outward_normal, ni_over_nt, refracted))
		{
			reflect_prob = schlick(cosine, ior);
		}
		else
		{
			reflect_prob = 1.0;
		}

		if (random() < reflect_prob)
		{
			scattered = ray3(record.position, reflected, in.time());
		}
		else
		{
			scattered = ray3(record.position, refracted, in.time());
		}

		return true;
	}

	virtual bool scatter3(const ray3& in, const trace_record& record, vec3& attenuation, ray3& scattered) const
	{
		attenuation = vec3(1.0, 1.0, 1.0);
		vec3 refracted;
		bool isRefracted = false;
		const vec3& n = record.normal;
		const vec3& i = in.direction();

		if (dot(i, n) > 0)
		{
			isRefracted = refract(i, -n, ior/ 1.0, refracted);
		}
		else
		{
			isRefracted = refract(i,  n, 1.0 / ior, refracted);
		}

		scattered = ray3(record.position, isRefracted ? refracted : reflect(i, n), in.time());

		return true;
	}

	virtual bool scatter(const ray3& in, const trace_record& record, vec3& atteunation, ray3& scattered) const
	{
		return scatter2(in, record, atteunation, scattered);
	}

	vec3 albedo;
	float ior;
};


#endif