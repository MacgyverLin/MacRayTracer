#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <float.h>
#include <math.h>
#include <iostream>

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

vec3 color2(const world3& world, const ray3& r)
{
	trace_record record;

	if (world.trace(r, 0.001, FLT_MAX, record))
	{
		vec3 target = record.position + record.normal + random_in_unit_sphere();
		return 0.5 * color2(world, ray3(record.position, target - record.position, r.time()));
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
		vec3 atteunation;

		vec3 emitted = record.mat->emit(0, 0, record.position);
		if (depth > 0 && record.mat->scatter(r, record, atteunation, scattered))
		{
			return emitted + atteunation * color3(world, scattered, depth - 1);
		}
		else
		{
			return emitted;
		}
	}
	else
	{
		return vec3(0.0, 0.0, 0.0);
		//        vec3 unit_direction = unit_vector(r.direction());
		//        float t = 0.5 * (unit_direction.y() + 1.0);
		//        return vec3(1.0, 1.0, 1.0) * (1.0 - t) + vec3(0.5, 0.7, 1.0) * t;
	}
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
		make_shared<sphere3>(vec3(0, 0, 0), -500.0, make_shared<diffuse_light>(vec3(0.0, 1.0, 1.0)))
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
//     objects.push_back(make_shared<sphere3>(vec3(3, 1, 2), 0.9, make_shared<dielectric>(vec3(0.8, 0.8, 0.8), 1.5)));

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

	shared_ptr<metal> metal_red = make_shared<metal>(vec3(0.65, 0.05, 0.05));
	shared_ptr<metal> metal_green = make_shared<metal>(vec3(0.12, 0.45, 0.15));
	shared_ptr<metal> metal_white = make_shared<metal>(vec3(0.73, 0.73, 0.73));

	shared_ptr<metal> rough_metal_red = make_shared<metal>(vec3(0.65, 0.05, 0.05), 0.3);
	shared_ptr<metal> rough_metal_green = make_shared<metal>(vec3(0.12, 0.45, 0.15), 0.3);
	shared_ptr<metal> rough_metal_white = make_shared<metal>(vec3(0.73, 0.73, 0.73), 0.3);
	
	shared_ptr<dielectric> glass_red = make_shared<dielectric>(vec3(0.65, 0.05, 0.05), 1.5);
	shared_ptr<dielectric> glass_green = make_shared<dielectric>(vec3(0.12, 0.45, 0.15), 1.5);
	shared_ptr<dielectric> glass_white = make_shared<dielectric>(vec3(0.73, 0.73, 0.73), 1.5);

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

	objects.push_back(make_shared<sphere3>(vec3(130, 165 + 120, 65), 120, glass_white));
	objects.push_back(make_shared<sphere3>(vec3(265, 330 + 120, 295), 120, metal_white));
	
	return make_shared<world3>(objects);
}

int raytrace()
{
	// Image Parameter
	const auto aspect_ratio = 16.0 / 9.0;
	const int image_width = 1920;
	const int image_height = static_cast<int>(image_width / aspect_ratio);
	const int samples_per_pixel = 8192;
	const int max_depth = 50;

	bitmap bmp(image_width, image_height);

	// world & camera
	camera3 camera;
	//shared_ptr<world3> world = scene1(camera, aspect_ratio);
	//shared_ptr<world3> world = scene2(camera, aspect_ratio, 4, 4, 2);
	//shared_ptr<world3> world = scene3(camera, aspect_ratio);
	shared_ptr<world3> world = scene_cornell_box(camera, aspect_ratio);

	for (int j = 0; j < image_height; j++)
	{
		for (int i = 0; i < image_width; i++)
		{
			vec3 color(0, 0, 0);
			for (int s = 0; s < samples_per_pixel; s++)
			{
				float u = float(i + random()) / float(image_width);
				float v = float(j + random()) / float(image_height);

				ray3 r = camera.get_ray(u, v);
				color += color3(*world, r, max_depth);
			}

			color /= samples_per_pixel;

			bmp.setPixel(i, j, color);
		}
		cout << "line:" << j << endl;
	}

	//bmp.tonemap();

	float gamma = 1.0 / 2.2;
	float vmax = 15;
	float A = 1.0 / pow(vmax, gamma);
	bmp.tonemap2(A, gamma);
	bmp.gamma_correction(1.0/2.2);
	bmp.saveBMP("1.bmp");
	bmp.savePPM("1.ppm");

	return 0;
}

int main()
{
	return raytrace();
}


/*
monte_carlo2(10000);
monte_carlo(10);
monte_carlo(100);
monte_carlo(1000);
monte_carlo(10000);
monte_carlo(20000);
*/

int monte_carlo(int sqrtN)
{
	double inside = 0;
	double inside_jittered = 0;

	for (int i = 0; i < sqrtN; i++)
	{
		for (int j = 0; j < sqrtN; j++)
		{
			float x = 2 * random() - 1;
			float y = 2 * random() - 1;
			if (x * x + y * y < 1)
				inside++;


			x = 2 * ((i + random()) / sqrtN) - 1;
			y = 2 * ((j + random()) / sqrtN) - 1;
			if (x * x + y * y < 1)
				inside_jittered++;
		}
	}

	cout << "-----------------------------" << endl;
	cout << "inside: " << 4 * double(inside) / (sqrtN * sqrtN) << endl;
	cout << "inside_jittered: " << 4 * double(inside_jittered) / (sqrtN * sqrtN) << endl;

	return 0;
}

int monte_carlo2(int N)
{
	float sum = 0;

	for (int i = 0; i < N; i++)
	{
		float x = 2 * random();
		sum += x * x;
	}

	cout << "-----------------------------" << endl;
	cout << "I: " << 2 * sum / N << endl;

	return 0;
}
