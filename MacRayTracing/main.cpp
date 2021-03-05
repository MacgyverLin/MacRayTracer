#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <float.h>
#include <math.h>
#include <iostream>
#include <chrono>
#include <ctime> 
#include <thread>

#include "util.h"
#include "vec3.h"
#include "ray3.h"
#include "bitmap.h"
#include "trace_record.h"
#include "world3.h"
#include "camera3.h"
#include "sphere3.h"
#include "box3.h"
#include "rect3.h"
#include "material.h"
#include "texture.h"
using namespace std;

int monte_carlo_1d(int N, float (*f)(float x), float (*pdf)(float x), float (*invPDF)(float y))
{
	float sum = 0;

	srand(0);
	for (int i = 0; i < N; i++)
	{
		float x = invPDF(random());
		sum += f(x) / pdf(x);
	}

	cout << sum / N << endl;

	return 0;
}

vec3 random_unit_vec3()
{
	auto a = random() * 2 * ONE_PI;
	auto z = random() * 2 - 1;
	auto r = sqrt(1 - z * z);
	return vec3(r * cos(a), r * sin(a), z);
}


int test_monte_carlo()
{
	cout << "uniform_monte_carlo:-----------------------------" << endl;
	for (int N = 1000; N < 20000; N += 100)
	{
		monte_carlo_1d
		(
			N,
			[](float x) -> float
		{
			return x * x;
		},
			[](float x) -> float
		{
			return float(1) / 2;
		},
			[](float y) -> float
		{
			return 2.0f * y;
		}
		);
	}

	cout << "non-uniform_monte_carlo:-----------------------------" << endl;
	for (int N = 1000; N < 20000; N += 100)
	{
		monte_carlo_1d
		(
			N,
			[](float x) -> float
		{
			return x * x;
		},
			[](float x) -> float
		{
			return x / 2;
		},
			[](float y) -> float
		{
			return sqrt(4 * y);
		}
		);
	}

	cout << "perfect-uniform_monte_carlo:-----------------------------" << endl;
	for (int N = 1000; N < 20000; N += 100)
	{
		monte_carlo_1d
		(
			N,
			[](float x) -> float
		{
			return x * x;
		},
			[](float x) -> float
		{
			return 3 * x * x / 8;
		},
			[](float y) -> float
		{
			return 8 * (pow(y, 1.0 / 3.0));
		}
		);
	}

	return -1;
}


////////////////////////////////
float hit_sphere(const vec3& center, float radius, const ray3& r)
{
	vec3 oc = r.origin() - center;
	float a = dot(r.direction(), r.direction());
	float b = 2.0 * dot(oc, r.direction());
	float c = dot(oc, oc) - radius * radius;
	float discriminant = b * b - 4 * a * c;

	if (discriminant < 0)
		return -1;
	else
		return (-b - sqrtf(discriminant)) / (2.0 * a);
}

vec3 color0(const ray3& r)
{
	float t = hit_sphere(vec3(0, 0, -1), 0.5, r);
	if (t > 0)
	{
		vec3 v1 = r.point_at_parameter(t);
		vec3 N = v1 - vec3(0, 0, -1);
		N = unit_vector(N);
		return 0.5 * (N + vec3(1, 1, 1));
		//return vec3(1, 0, 0);
	}
	else
	{
		vec3 unit_direction = unit_vector(r.direction());
		float t = 0.5 * (unit_direction.y() + 1.0);
		return vec3(1.0, 1.0, 1.0) * (1.0 - t) + vec3(0.5, 0.7, 1.0) * t;
	}
}

vec3 color1(const world3& world, const ray3& r)
{
	trace_record record;

	if (world.trace(r, 0.001, FLT_MAX, record))
	{
		return 0.5 * (record.normal + vec3(1, 1, 1));
	}
	else
	{
		vec3 unit_direction = unit_vector(r.direction());
		float t = 0.5 * (unit_direction.y() + 1.0);
		return vec3(1.0, 1.0, 1.0) * (1.0 - t) + vec3(0.5, 0.7, 1.0) * t;
	}
}

vec3 color2(const world3& world, const ray3& r, int depth = 10)
{
	trace_record record;

	if (depth>0 && world.trace(r, 0.001, FLT_MAX, record))
	{
		vec3 target = record.position + record.normal + random_in_unit_sphere();
		return 0.5 * color2(world, ray3(record.position, target - record.position, r.time()), depth - 1);
	}
	else
	{
		vec3 unit_direction = unit_vector(r.direction());
		float t = 0.5 * (unit_direction.y() + 1.0);
		return vec3(1.0, 1.0, 1.0) * (1.0 - t) + vec3(0.5, 0.7, 1.0) * t;
	}
}

vec3 color3(const world3& world, const ray3& r, int depth = 50)
{
	trace_record record;
	if (world.trace(r, 0.001, FLT_MAX, record))
	{
		ray3 scattered;
		vec3 emitted = record.mat->emit(0, 0, record.position);
		float pdf;
		vec3 albedo;

		if (depth > 0 && record.mat->scatter(r, record, albedo, scattered, pdf))
		{
			return emitted + albedo * record.mat->scatter_pdf(r, record, scattered) * color3(world, scattered, depth - 1) / pdf / 5;
		}
		else
		{
			return emitted;
		}
	}
	else
	{
		//return vec3(0.0, 0.0, 0.0);
		vec3 unit_direction = unit_vector(r.direction());
		float t = 0.5 * (unit_direction.y() + 1.0);
		return vec3(1.0, 1.0, 1.0) * (1.0 - t) + vec3(0.5, 0.7, 1.0) * t;
	}
}

vec3 color41(const world3& world, const ray3& ray, int depth)
{
	trace_record record;
	if (depth > 0 && world.trace(ray, 0.001, FLT_MAX, record))
	{
		vec3 target = record.position + record.normal + random_in_unit_sphere();
		return 0.5 * color41(world, ray3(record.position, target - record.position, ray.time()), depth - 1);
	}
	else
	{
		//return vec3(0.0, 0.0, 0.0);
		vec3 unit_direction = unit_vector(ray.direction());
		float t = 0.5 * (unit_direction.y() + 1.0);
		return vec3(1.0, 1.0, 1.0) * (1.0 - t) + vec3(0.5, 0.7, 1.0) * t;
	}
}

vec3 color42(const world3& world, const ray3& ray, int depth)
{
	ray3 newray = ray;

	trace_record record;

	vec3 frac(1.0, 1.0, 1.0);
	vec3 finalColor(0.0, 0.0, 0.0);
	while(depth > 0)
	{
		depth--;
		if (world.trace(newray, 0.001, FLT_MAX, record))
		{
			frac *= 0.5;

			newray.o = record.position;
			newray.d = record.normal + random_in_unit_sphere();
		}
		else
		{
			vec3 unit_direction = unit_vector(newray.direction());
			float t = 0.5 * (unit_direction.y() + 1.0);
			finalColor = vec3(1.0, 1.0, 1.0) * (1.0 - t) + vec3(0.5, 0.7, 1.0) * t;

			break;
		}
	}

	vec3 result = finalColor * frac;
	return result;
}

vec3 color4(const world3& world, ray3& ray, int depth)
{
	//return color41(world, ray, depth);
	return color42(world, ray, depth);
}

shared_ptr<world3> scene0(camera3& camera, float aspect_ratio)
{
	vec3 lookfrom(0, 0, 3);
	vec3 lookat(0, 0, -1);
	vec3 vup(0, 1, 0);
	auto dist_to_focus = 3.0;
	auto aperture = 0.0;
	camera = camera3(lookfrom, lookat, vup, 20, aspect_ratio, aperture, dist_to_focus, 0, 1.0f / 60.0f * 3);

	vector<shared_ptr<traceable3>> objects;
	objects.push_back(make_shared<sphere3>(vec3(0, 0, -1), 0.5, make_shared<metal>(vec3(0.8, 0.6, 0.2))));
	objects.push_back(make_shared<sphere3>(vec3(0, -100.5, -1), 100, make_shared<lambertian>(make_shared<const_texture>(vec3(0.8, 0.8, 0.0)))));

	objects.push_back
	(
		make_shared<sphere3>(vec3(0, 8, 0), 5.0, make_shared<diffuse_light>(vec3(30.0, 30.0, 30.0)))
	);

	return make_shared<world3>(objects);
}

shared_ptr<world3> scene1(camera3& camera, float aspect_ratio)
{
	vec3 lookfrom(13, 2, 3);
	vec3 lookat(0, 0, 0);
	vec3 vup(0, 1, 0);
	auto dist_to_focus = 10.0;
	auto aperture = 0.1;
	camera = camera3(lookfrom, lookat, vup, 20, aspect_ratio, aperture, dist_to_focus, 0, 1.0f / 60.0f * 3);

	vector<shared_ptr<traceable3>> objects;
	objects.push_back(make_shared<sphere3>(vec3(0, -100.5, -1), 100, make_shared<lambertian>(make_shared<const_texture>(vec3(0.8, 0.8, 0.0)))));
	objects.push_back(make_shared<sphere3>(vec3(1, 0, -1), 0.5, make_shared<metal>(vec3(0.8, 0.6, 0.2))));
	objects.push_back(make_shared<sphere3>(vec3(-1, 0, -1), 0.5, make_shared<dielectric>(vec3(1.0, 1.0, 1.0), 1.5)));
	objects.push_back(make_shared<sphere3>(vec3(-1, 0, -1), -0.49, make_shared<dielectric>(vec3(1.0, 1.0, 1.0), 1.5)));
	objects.push_back(make_shared<sphere3>(vec3(0, 0, -1), 0.5, make_shared<lambertian>(make_shared<const_texture>(vec3(0.1, 0.2, 0.5)))));

	objects.push_back
	(
		make_shared<sphere3>(vec3(0, 8, 0), 5.0, make_shared<diffuse_light>(vec3(30.0, 30.0, 30.0)))
	);

	return make_shared<world3>(objects);
}

shared_ptr<world3> scene2(camera3& camera, float aspect_ratio, int lamberts, int metals, int glasses)
{
	/////////////////////////////////////////////////////////////////////////////////////////////
	// camera
	vec3 lookfrom(13, 2, 3);
	vec3 lookat(0, 0, 0);
	vec3 vup(0, 1, 0);
	float dist_to_focus = 10.0;
	float aperture = 0.1;
	float vfov = 20.0;
	camera = camera3(lookfrom, lookat, vup, vfov, aspect_ratio, aperture, dist_to_focus, 0, 1.0f / 60.0f * 3);

	/////////////////////////////////////////////////////////////////////////////////////////////
	// world
	vector<shared_ptr<traceable3>> objects;

	float total = lamberts + metals + glasses;
	float ratio0 = (float)((lamberts)) / total;
	float ratio1 = (float)((lamberts + metals)) / total;
	float ratio2 = (float)((lamberts + metals + glasses)) / total;

	auto ground_mat = make_shared<lambertian>
	(
		make_shared<checker_texture>
		(
			make_shared<const_texture>(vec3(0.2, 0.3, 0.1)),
			make_shared<noise_texture>(),
			vec3(10.0, 10.0, 10.0)
		)
	);
	objects.push_back
	(
		make_shared<sphere3>(vec3(0, -1000, 0), 1000, ground_mat)
	);

	for (int z = -11; z < 11; z++)
	{
		for (int x = -11; x < 11; x++)
		{
			float material_sel = random();

			float radius = 0.2;// +random() * 0.1;
			vec3 center(x + random() * -0.5, radius, z + random() * -0.5);
			vec3 velocity = vec3(random(), random(), random()) * 0.0f;
			if (material_sel <= ratio0) // lambertian
			{
				auto lambertian_mat = make_shared<lambertian>
				(
					make_shared<const_texture>
					(
						vec3(random(), random(), random())
					)
				);

				objects.push_back
				(
					make_shared<sphere3>(center, radius, lambertian_mat, velocity)
				);
			}
			else if (material_sel <= ratio1) // metal
			{
				auto metal_mat = make_shared<metal>
				(
					vec3
					(
						0.5 + 0.5 * random(),
						0.5 + 0.5 * random(),
						0.5 + 0.5 * random()
					),
					random() * 0.5
				);
				objects.push_back
				(
					make_shared<sphere3>(center, radius, metal_mat, velocity)
				);
			}
			else if (material_sel <= ratio2) // dielectric
			{
				auto dielectric_mat = make_shared<dielectric>
				(
					vec3
					(
						1,
						1,
						1
					),
					1.5
				);
				objects.push_back
				(
					make_shared<sphere3>(center, radius, dielectric_mat, velocity)
				);
			}
		}
	}

	objects.push_back
	(
		make_shared<sphere3>(vec3(-6, 1, 0), 1.0, make_shared<lambertian>(make_shared<const_texture>(vec3(0.4, 0.2, 0.1))))
	);
	objects.push_back
	(
		make_shared<sphere3>(vec3(0, 1, 0), 1.0, make_shared<dielectric>(vec3(1, 1, 1), 1.5))
	);
	objects.push_back
	(
		make_shared<sphere3>(vec3(6, 1, 0), 1.0, make_shared<metal>(vec3(0.7, 0.6, 0.5), 0.0))
	);

	shared_ptr<image_texture> img_tex = make_shared< image_texture>();
	if (!img_tex->load("earthmap.jpg"))
		return nullptr;
	objects.push_back
	(
		make_shared<sphere3>(vec3(6, 1, 2.5), 0.75, make_shared<lambertian>(img_tex))
	);
	objects.push_back
	(
		make_shared<sphere3>(vec3(6, 3, 2.5), 0.75, make_shared<lambertian>(img_tex))
	);

	objects.push_back
	(
		make_shared<sphere3>(vec3(0, 0, 0), -500.0, make_shared<diffuse_light>(vec3(10.0, 10.0, 10.0)))
	);

	//objects.push_back
	//(
		//make_shared<sphere3>(vec3(-6, 8, 0), 5.0, make_shared<light>(vec3(30.0, 30.0, 30.0)))
	//);

	return make_shared<world3>(objects);
}

shared_ptr<world3> scene3(camera3& camera, float aspect_ratio)
{
	vec3 lookfrom(13, 2, 3);
	vec3 lookat(0, 0, 0);
	vec3 vup(0, 1, 0);
	float dist_to_focus = 10.0;
	float aperture = 0.0;
	float vfov = 20.0;
	camera = camera3(lookfrom, lookat, vup, vfov, aspect_ratio, aperture, dist_to_focus, 0, 1.0f);

	shared_ptr<noise_texture> perlin_tex = make_shared<noise_texture>(5);
	shared_ptr<image_texture> image_tex = make_shared<image_texture>();
	if (!image_tex->load("earthmap.jpg"))
		return nullptr;

	vector<shared_ptr<traceable3>> objects;
	objects.push_back(make_shared<sphere3>(vec3(0, -1000, 0), 1000, make_shared<lambertian>(perlin_tex)));
	objects.push_back(make_shared<sphere3>(vec3(0, 2, 0), 2, make_shared<lambertian>(perlin_tex)));
	// objects.push_back(make_shared<sphere3>(vec3(3, 1, -2), 0.9, make_shared<lambertian>(const_tex)));
	// objects.push_back(make_shared<sphere3>(vec3(3, 1, 0), 0.9, make_shared<metal>(vec3(0.8, 0.8, 0.8))));
	// objects.push_back(make_shared<sphere3>(vec3(3, 1, 2), 0.9, make_shared<dielectric>(vec3(0.8, 0.8, 0.8), 1.5)));

	objects.push_back
	(
		make_shared<sphere3>(vec3(0, 8, 0), 5.0, make_shared<diffuse_light>(vec3(30.0, 30.0, 30.0)))
	);

	return make_shared<world3>(objects);
}

shared_ptr<world3> scene_cornell_box(camera3& camera, float aspect_ratio)
{
	vec3 lookfrom(278, 278, -800);
	vec3 lookat(278, 278, 0);
	vec3 vup(0, 1, 0);
	float dist_to_focus = 10.0;
	float aperture = 0.0;
	float vfov = 40.0;
	camera = camera3(lookfrom, lookat, vup, vfov, aspect_ratio, aperture, dist_to_focus, 0, 1.0);


	vector<shared_ptr<traceable3>> objects;
	shared_ptr<diffuse_light> light = make_shared<diffuse_light>(vec3(15, 15, 15));

	shared_ptr<lambertian> lambert_red = make_shared<lambertian>(make_shared<const_texture>(vec3(0.65, 0.05, 0.05)));
	shared_ptr<lambertian> lambert_green = make_shared<lambertian>(make_shared<const_texture>(vec3(0.12, 0.45, 0.15)));
	shared_ptr<lambertian> lambert_white = make_shared<lambertian>(make_shared<const_texture>(vec3(0.73, 0.73, 0.73)));

	shared_ptr<transmissive> transmissive_red = make_shared<transmissive>(make_shared<const_texture>(vec3(0.65, 0.05, 0.05)), 1.5f);
	shared_ptr<transmissive> transmissive_green = make_shared<transmissive>(make_shared<const_texture>(vec3(0.12, 0.45, 0.15)), 1.5f);
	shared_ptr<transmissive> transmissive_white = make_shared<transmissive>(make_shared<const_texture>(vec3(0.73, 0.73, 0.73)), 1.5f);
	
	shared_ptr<metal> metal_red = make_shared<metal>(vec3(0.65, 0.05, 0.05));
	shared_ptr<metal> metal_green = make_shared<metal>(vec3(0.12, 0.45, 0.15));
	shared_ptr<metal> metal_white = make_shared<metal>(vec3(0.73, 0.73, 0.73));

	shared_ptr<metal> rough_metal_red = make_shared<metal>(vec3(0.65, 0.05, 0.05), 0.3);
	shared_ptr<metal> rough_metal_green = make_shared<metal>(vec3(0.12, 0.45, 0.15), 0.3);
	shared_ptr<metal> rough_metal_white = make_shared<metal>(vec3(0.73, 0.73, 0.73), 0.3);
	
	shared_ptr<dielectric> glass_red = make_shared<dielectric>(vec3(0.65, 0.05, 0.05), 1.5);
	shared_ptr<dielectric> glass_green = make_shared<dielectric>(vec3(0.12, 0.45, 0.15), 1.5);
	shared_ptr<dielectric> glass_white = make_shared<dielectric>(vec3(0.73, 0.73, 0.73), 1.5);

	shared_ptr<const_texture> texture_red = make_shared<const_texture>(vec3(0.65, 0.05, 0.05));
	shared_ptr<const_texture> texture_green = make_shared<const_texture>(vec3(0.12, 0.45, 0.15));
	shared_ptr<const_texture> texture_white = make_shared<const_texture>(vec3(0.73, 0.73, 0.73));

	objects.push_back(make_shared<xz_rect>(555 / 2 - 150, 555 / 2 + 150, 555 / 2 - 150, 555 / 2 + 150, 554, light));

	objects.push_back(make_shared<flip_normal>(make_shared<yz_rect>(0, 555, 0, 555, 555, lambert_green)));
	objects.push_back(make_shared<yz_rect>(0, 555, 0, 555, 0, lambert_red));
	objects.push_back(make_shared<flip_normal>(make_shared<xz_rect>(0, 555, 0, 555, 555, lambert_white)));
	objects.push_back(make_shared<xz_rect>(0, 555, 0, 555, 0, lambert_white));
	objects.push_back(make_shared<flip_normal>(make_shared<xy_rect>(0, 555, 0, 555, 555, lambert_white)));

	mat4 m0;
	m0.init_translate_rot_zxy_scale(130, 0, 65, 0, 0, -18, 1);
	objects.push_back(make_shared<transform_node>(make_shared<box3>(vec3(0, 0, 0), vec3(165, 165, 165), lambert_white), m0));

	mat4 m1;
	m1.init_translate_rot_zxy_scale(265, 0, 295, 0, 0, 15, 1);
	objects.push_back(make_shared<transform_node>(make_shared<box3>(vec3(0, 0, 0), vec3(165, 330, 165), lambert_white), m1));
	
	// objects.push_back(make_shared<const_medium>(make_shared<sphere3>(vec3(130, 165 + 480, 65), 480, glass_white), 0.01, texture_red));
	// objects.push_back(make_shared<const_medium>(make_shared<sphere3>(vec3(265, 330 + 480, 295), 480, metal_white), 0.01, texture_green));
	objects.push_back(make_shared<sphere3>(vec3(130, 165 + 120, 65), 120, glass_white));
	objects.push_back(make_shared<sphere3>(vec3(265, 330 + 120, 295), 120, metal_white));
	
	return make_shared<world3>(objects);
}

shared_ptr<world3> scene4(camera3& camera, float aspect_ratio)
{
	vec3 lookfrom(0, 0, 20);
	vec3 lookat(0, 0, 0);
	vec3 vup(0, 1, 0);
	auto dist_to_focus = 10.0;
	auto aperture = 0.1;
	camera = camera3(lookfrom, lookat, vup, 20, aspect_ratio, aperture, dist_to_focus, 0, 1.0f / 60.0f * 3);

	shared_ptr<lambertian> material = make_shared<lambertian>(make_shared<const_texture>(vec3(0.8, 0.8, 0.8)));

	vector<shared_ptr<traceable3>> objects;

	for (int y = -10; y < 10; y++)
	{
		for (int x = -10; x < 10; x++)
		{
			objects.push_back(make_shared<sphere3>(vec3(x*1.5, y * 1.5, 0), 0.5, material));
		}
	}
		
	objects.push_back
	(
		make_shared<sphere3>(vec3(0, 10, 0), 3.0, make_shared<diffuse_light>(vec3(30.0, 30.0, 30.0)))
	);

	return make_shared<world3>(objects);
}

class Render
{
public:
	Render
	(
		bitmap& bmp_,
		camera3& camera_,
		world3& world_,
		int x_,
		int y_,
		int samples_per_pixel_,
		float image_width_,
		float image_height_,
		int max_depth_
	)
	: bmp(bmp_)
	, camera(camera_)
	, world(world_)
	, x(x_)
	, y(y_)
	, samples_per_pixel(samples_per_pixel_)
	, image_width(image_width_)
	, image_height(image_height_)
	, max_depth(max_depth_)
	{
	}

	bitmap& bmp;
	camera3& camera;
	world3& world;
	int x;
	int y;
	int samples_per_pixel;
	float image_width;
	float image_height;
	int max_depth;
};

void render_pixel(const Render& rend, int offset)
{
	vec3 color(0, 0, 0);
	for (int s = 0; s < rend.samples_per_pixel; s++)
	{
		float u = float(rend.x + offset + random()) / float(rend.image_width);
		float v = float(rend.y + random()) / float(rend.image_height);

		ray3 r = rend.camera.get_ray(u, v);
		color += color3(rend.world, r, rend.max_depth);
	}

	color /= rend.samples_per_pixel;

	rend.bmp.setPixel(rend.x + offset, rend.y, color);
}

int raytrace(int image_width, float aspect_ratio, int samples_per_pixel)
{
	//for (int i = 0; i < 10000; i++)
	//{
	//	printf("%d %f\n", i, GetUniform());
	//}
	
	// Image Parameter
	//const auto aspect_ratio = 16.0 / 9.0;
	//const int image_width = 1080;
	//const int samples_per_pixel = 8192;
	const int image_height = static_cast<int>(image_width / aspect_ratio);	
	const int max_depth = 10;

	bitmap bmp(image_width, image_height);

	auto start = std::chrono::system_clock::now();

	// world & camera
	camera3 camera;
	shared_ptr<world3> world = scene0(camera, aspect_ratio);
	//shared_ptr<world3> world = scene1(camera, aspect_ratio);
	//shared_ptr<world3> world = scene2(camera, aspect_ratio, 4, 4, 2);
	//shared_ptr<world3> world = scene3(camera, aspect_ratio);
	//shared_ptr<world3> world = scene_cornell_box(camera, aspect_ratio);
	//shared_ptr<world3> world = scene4(camera, aspect_ratio);

	auto prepare = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_seconds = prepare - start;
	printf("%f\n", elapsed_seconds.count());

	//world->print(0);
	printf("Mac Lin's Ray Tracer @COPYRIGHT 2020\nRayTracing....\n");

	Render render
	(
		bmp,
		camera,
		*world.get(),
		0,
		0,
		samples_per_pixel,
		image_width,
		image_height,
		max_depth
	);

	int max_thread = 1;// std::thread::hardware_concurrency();

	for (int j = 0; j < image_height; j++)
	{
		int ii = 0;
		cout << "line: " << j << " ";
		for (int i = 0; i < image_width; i+= max_thread)
		{
			vec3 color(0, 0, 0);
			for (int s = 0; s < samples_per_pixel; s++)
			{
				float u = float(i + random()) / float(image_width);
				float v = float(j + random()) / float(image_height);

				ray3 r = camera.get_ray(u, v);
				color += color4(*world.get(), r, max_depth);
			}
			
			color /= samples_per_pixel;

			bmp.setPixel(i, j, color);

			/*
			render.x = i;
			render.y = j;
			std::vector<std::thread> render_queue;
			for(int k =0; k< max_thread; k++)
				render_queue.push_back(std::thread(render_pixel, render, k));

			for (int k = 0; k < max_thread; k++)
				render_queue[k].join();
			*/

			if ((ii++) % (image_width/40) == 0)
				cout << ".";
		}
		cout << endl;
	}

	auto end = std::chrono::system_clock::now();
	elapsed_seconds = end - prepare;
	printf("%f\n", elapsed_seconds.count());

	float gamma = 1.0 / 2.2;
	float vmax = 15;
	float a = 1.0 / pow(vmax, gamma);
	bmp.tonemap2(a, gamma);
	bmp.gamma_correction(1.0/2.2);
	bmp.saveBMP("1.bmp");
	bmp.savePPM("1.ppm");

	return 0;
}

int main(int argc, char* argv[])
{
	//return makeColor(256, 256);
	return raytrace(400, 1.7777, 50);
	//return raytrace(atof(argv[1]), atoi(argv[2]), atoi(argv[3]));

	// return test_monte_carlo();
}