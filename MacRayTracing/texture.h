#ifndef _TEXTURE_h_
#define _TEXTURE_h_

#include "vec3.h"

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

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

class image_texture : public texture {
public:
	image_texture()
	: width(0)
	, height(0)
	{
	}

	~image_texture() {
	}

	bool load(const char* filename)
	{
		auto components_per_pixel = 3;

		unsigned char* rawdata = stbi_load(filename, &width, &height, &components_per_pixel, components_per_pixel);
		if (!rawdata)
		{
			std::cerr << "ERROR: Could not load texture image file '" << filename << "'.\n";
			width = height = 0;
			return false;
		}

		buffer.resize(width * height);
		for (int i = 0; i < width * height; i++)
		{
			int idx = i * 3;

			buffer[i][0] = float(rawdata[idx + 0]) / 255;
			buffer[i][1] = float(rawdata[idx + 1]) / 255;
			buffer[i][2] = float(rawdata[idx + 2]) / 255;
		}

		delete rawdata;

		return true;
	}

	void texcoord_clamp(float& i, float& j) const
	{
		i = clamp(i, 0, 1);
		j = clamp(j, 0, 1);
	}

	void texcoord_repeat(float& i, float& j) const
	{
		i = fmod(i, 1);
		j = fmod(j, 1);
	}

	void texcoord_repeat_mirror(float& i, float& j) const
	{
		i = fmod(i, 2);
		j = fmod(j, 2);

		i = (i < 1) ? i : 2 - i;
		j = (j < 1) ? j : 2 - j;
	}

	virtual vec3 sample(float u, float v, const vec3& p) const
	{
		// Clamp input texture coordinates to [0,1] x [1,0]
		u = clamp(u, 0.0, 1.0);
		v = 1.0 - clamp(v, 0.0, 1.0);  // Flip V to image coordinates

		texcoord_clamp(u, v);

		int i = u * (width - 1);
		int j = v * (height - 1);
		return buffer[j * width + i];
	}
private:
	vector<vec3> buffer;
	int width, height;
};

#endif