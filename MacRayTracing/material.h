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
	static bool inited;
	static vec3 randomvec3[256];
	static int perm_x[256];
	static int perm_y[256];
	static int perm_z[256];

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

	static void perlin_generate()
	{
		for (int i = 0; i < 256; i++)
		{
			randomvec3[i] = unit_vector
			(
				vec3
				(
					-1 + 2 * random(),
					-1 + 2 * random(),
					-1 + 2 * random()
				)
			);
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

	//////////////////////////////////////////////////////////////
	float linear_interp(const vec3& p) const
	{
		float u = p.x() - floor(p.x());
		float v = p.y() - floor(p.y());
		float w = p.z() - floor(p.z());
		int i = floor(p.x());
		int j = floor(p.y());
		int k = floor(p.z());

		vec3 c[2][2][2];
		for (int di = 0; di < 2; di++)
		{
			for (int dj = 0; dj < 2; dj++)
			{
				for (int dk = 0; dk < 2; dk++)
				{
					c[di][dj][dk] = randomvec3[perm_x[(i + di) & 255] ^ perm_y[(j + dj) & 255] ^ perm_z[(k + dk) & 255]];
				}
			}
		}

		/////////////////////////////////////////////////////
		float accum = 0;
		for (int i = 0; i < 2; i++)
		{
			for (int j = 0; j < 2; j++)
			{
				for (int k = 0; k < 2; k++)
				{
					vec3 weight(u - i, v - j, w - k);
					accum +=
						((i * u) + (1 - i) * (1 - u)) *
						((j * v) + (1 - j) * (1 - v)) *
						((k * w) + (1 - k) * (1 - w)) *
						dot(c[i][j][k], weight);
				}
			}
		}

		return accum;
	}

	float cubic_interp(const vec3& p) const
	{
		float u = p.x() - floor(p.x());
		float v = p.y() - floor(p.y());
		float w = p.z() - floor(p.z());
		int i = floor(p.x());
		int j = floor(p.y());
		int k = floor(p.z());

		vec3 c[2][2][2];
		for (int di = 0; di < 2; di++)
		{
			for (int dj = 0; dj < 2; dj++)
			{
				for (int dk = 0; dk < 2; dk++)
				{
					c[di][dj][dk] = randomvec3[perm_x[(i + di) & 255] ^ perm_y[(j + dj) & 255] ^ perm_z[(k + dk) & 255]];
				}
			}
		}

		/////////////////////////////////////////////////////
		float uu = u * u * (3 - 2 * u);
		float vv = v * v * (3 - 2 * v);
		float ww = w * w * (3 - 2 * w);
		float accum = 0;
		for (int i = 0; i < 2; i++)
		{
			for (int j = 0; j < 2; j++)
			{
				for (int k = 0; k < 2; k++)
				{
					vec3 weight(u - i, v - j, w - k);
					accum +=
						((i * uu) + (1 - i) * (1 - uu)) *
						((j * vv) + (1 - j) * (1 - vv)) *
						((k * ww) + (1 - k) * (1 - ww)) *
						dot(c[i][j][k], weight);
				}
			}
		}

		return accum;
	}

	float value(const vec3& p, int filtering = 0) const
	{
		if (filtering == 0)
			return cubic_interp(p);
		else
			return linear_interp(p);
	}

	float turbulance(const vec3& p, int depth = 7) const
	{
		float accum = 0;
		vec3 next_position = p;
		float next_weight = 1;
		for (int i = 0; i < depth; i++)
		{
			accum += next_weight * value(next_position);
			next_weight *= 0.5;
			next_position *= 2;
		}

		return fabs(accum);
	}
};

bool perlin::inited = false;
vec3 perlin::randomvec3[256];
int perlin::perm_x[256];
int perlin::perm_y[256];
int perlin::perm_z[256];

class noise_texture : public texture
{
public:
	noise_texture(float scale_ = 4.0)
	: noise()
	, scale(scale_)
	{
	}

	virtual vec3 sample(float u, float v, const vec3& p) const
	{
		//return vec3(1, 1, 1) * 0.5 * (1 + noise.value(p * scale));
		//return vec3(1, 1, 1) * noise.value(p * scale);
		// return vec3(1, 1, 1) * 0.5 * (1 + noise.turbulance(p * scale));
		//return vec3(1, 1, 1) * noise.turbulance(p * scale);
		return vec3(1, 1, 1) * 0.5 * (1 + sin(scale*p.z() + 10 * noise.turbulance(p)));
	}

	float scale;
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

class image_texture : public texture {
public:
	const static int bytes_per_pixel = 3;

	image_texture()
		: data(nullptr), width(0), height(0), bytes_per_scanline(0) {}

	image_texture(const char* filename) {
		auto components_per_pixel = bytes_per_pixel;

		data = stbi_load(
			filename, &width, &height, &components_per_pixel, components_per_pixel);

		if (!data) {
			std::cerr << "ERROR: Could not load texture image file '" << filename << "'.\n";
			width = height = 0;
		}

		bytes_per_scanline = bytes_per_pixel * width;
	}

	~image_texture() {
		delete data;
	}

	virtual color value(double u, double v, const vec3& p) const override {
		// If we have no texture data, then return solid cyan as a debugging aid.
		if (data == nullptr)
			return color(0, 1, 1);

		// Clamp input texture coordinates to [0,1] x [1,0]
		u = clamp(u, 0.0, 1.0);
		v = 1.0 - clamp(v, 0.0, 1.0);  // Flip V to image coordinates

		auto i = static_cast<int>(u * width);
		auto j = static_cast<int>(v * height);

		// Clamp integer mapping, since actual coordinates should be less than 1.0
		if (i >= width)  i = width - 1;
		if (j >= height) j = height - 1;

		const auto color_scale = 1.0 / 255.0;
		auto pixel = data + j * bytes_per_scanline + i * bytes_per_pixel;

		return color(color_scale * pixel[0], color_scale * pixel[1], color_scale * pixel[2]);
	}

private:
	unsigned char* data;
	int width, height;
	int bytes_per_scanline;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class material
{
public:
	material()
	{
	}

	virtual bool scatter(const ray3& in, const trace_record& record, vec3& atteunation, ray3& scattered) const = 0;

	virtual vec3 emit(float u, float v, const vec3& p) const = 0;
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

	virtual vec3 emit(float u, float v, const vec3& p) const
	{
		return vec3(0, 0, 0);
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

	virtual vec3 emit(float u, float v, const vec3& p) const
	{
		return vec3(0, 0, 0);
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

	virtual vec3 emit(float u, float v, const vec3& p) const
	{
		return vec3(0, 0, 0);
	}

	vec3 albedo;
	float ior;
};

class light : public material
{
public:
	light(const vec3& color_)
	{
		color = color_;
	}

	virtual bool scatter(const ray3& in, const trace_record& record, vec3& atteunation, ray3& scattered) const
	{
		return false;
	}

	virtual vec3 emit(float u, float v, const vec3& p) const
	{
		return color;
	}

	vec3 color;
	/*
	light(shared_ptr<texture> color)
	{
		this->color = color;
	}
	shared_ptr<texture> color;
	*/

};

class light_texture : public material
{
public:
	light_texture(shared_ptr<texture> color)
	{
		this->color = color;
	}

	virtual bool scatter(const ray3& in, const trace_record& record, vec3& atteunation, ray3& scattered) const
	{
		return false;
	}

	virtual vec3 emit(float u, float v, const vec3& p) const
	{
		return color->sample(u, v, p);
	}

	shared_ptr<texture> color;
};

#endif