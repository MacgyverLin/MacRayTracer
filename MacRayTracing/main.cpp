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
#include "material.h"
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
    if(t > 0)
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
        return 0.5*(record.normal + vec3(1, 1, 1));
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

vec3 color3(const world3& world, const ray3& r, int depth)
{
    trace_record record;
    if (world.trace(r, 0.001, FLT_MAX, record))
    {
        ray3 scattered;
        vec3 atteunation;

        if (depth > 0 && record.mat->scatter(r, record, atteunation, scattered))
        {
            return atteunation * color3(world, scattered, depth - 1);
        }
        else
            return vec3(0, 0, 0);
    }
    else
    {
        vec3 unit_direction = unit_vector(r.direction());
        float t = 0.5 * (unit_direction.y() + 1.0);
        return vec3(1.0, 1.0, 1.0) * (1.0 - t) + vec3(0.5, 0.7, 1.0) * t;
    }
}

vec3 gamma_correction(const vec3& c, float gamma)
{
    return vec3(pow(c[0], gamma), pow(c[1], gamma), pow(c[2], gamma));
}

void scene1(world3& world)
{
    world.add(new sphere3(vec3(0, -100.5, -1), 100, new lambertian(vec3(0.8, 0.8, 0.0))));
    world.add(new sphere3(vec3(1, 0, -1), 0.5, new metal(vec3(0.8, 0.6, 0.2))));
    world.add(new sphere3(vec3(-1, 0, -1), 0.5, new dielectric(vec3(1.0, 1.0, 1.0), 1.5)));
    world.add(new sphere3(vec3(-1, 0, -1), -0.49, new dielectric(vec3(1.0, 1.0, 1.0), 1.5)));
    world.add(new sphere3(vec3(0, 0, -1), 0.5, new lambertian(vec3(0.1, 0.2, 0.5))));
}

void scene2(world3& world, int lamberts, int metals, int glasses)
{
    float total = lamberts + metals + glasses;
    float ratio0 = (float)((lamberts)) / total;
    float ratio1 = (float)((lamberts + metals)) / total;
    float ratio2 = (float)((lamberts + metals + glasses)) / total;


    world.add(new sphere3(vec3(0, -1000, 0), 1000, new lambertian(vec3(0.5, 0.5, 0.5))));
    for (int z = -11; z < 11; z++)
    {
        for (int x = -11; x < 11; x++)
        {
            float material_sel = random();
            
            vec3 center(x + random() * - 0.5, 0.2, z + random() * - 0.5);
            if (material_sel <= ratio0) // diffuse
            {
                world.add
                (
                    new sphere3
                    (
                        center, 
                        0.2, 
                        new lambertian(vec3(random(), random(), random())),
                        vec3(random() * 3, random() * 3, random() * 3)
                    )
                );
            }
            else if (material_sel <= ratio1) // metal
            {
                world.add
                (
                    new sphere3
                    (
                        center, 
                        0.2, 
                        new metal(vec3(0.5+ 0.5*random(), 0.5 + 0.5 * random(), 0.5 + 0.5 * random()), random() * 0.5),
                        vec3(random() * 3, random() * 3, random() * 3)
                    )
                );
            }
            else if (material_sel <= ratio2) // dielectric
            {
                world.add
                (
                    new sphere3
                    (
                        center, 
                        0.2, 
                        new dielectric(vec3(1, 1, 1), 1.5),
                        vec3(random() * 3, random() * 3, random() * 3)
                    )
                );
            }
        }
    }

    world.add(new sphere3(vec3( 0, 1, 0), 1.0, new dielectric(vec3(1, 1, 1), 1.5)));
    world.add(new sphere3(vec3(-6, 1, 0), 1.0, new lambertian(vec3(0.4, 0.2, 0.1))));
    world.add(new sphere3(vec3( 6, 1, 0), 1.0, new metal(vec3(0.7, 0.6, 0.5), 0.0)));    
}

int main()
{
    // Image Parameter
    const auto aspect_ratio = 16.0 / 9.0;
    const int image_width = 800;
    const int image_height = static_cast<int>(image_width / aspect_ratio);
    const int samples_per_pixel = 32;
    const int max_depth = 50;
    
    bitmap bmp(image_width, image_height);

    // world
    world3 world;
    // scene1(world);
    scene2(world, 4, 4, 2);

    // camera
    vec3 lookfrom(13, 2, 3);
    vec3 lookat(0, 0, 0);
    vec3 vup(0, 1, 0);
    auto dist_to_focus = 10.0;
    auto aperture = 0.1;
    camera3 camera(lookfrom, lookat, vup, 20, aspect_ratio, aperture, dist_to_focus, 0, 1.0f/60.0f*3);

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
                color += color3(world, r, max_depth);
            }

            color /= samples_per_pixel;
            color = gamma_correction(color, 1.0/2.2f);

            bmp.setPixel(i, j, floor(255 * color[0]), floor(255 * color[1]), floor(255 * color[2]));
        }
        std::cout << "line:" << j << std::endl;
    }

    bmp.saveBMP("1.bmp");
    bmp.savePPM("1.ppm");

    return 0;
}