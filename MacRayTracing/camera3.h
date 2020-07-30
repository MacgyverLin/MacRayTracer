#ifndef _CAMERA3_h_
#define _CAMERA3_h_

#include "vec3.h"
#include "ray3.h"

class camera3
{
public:
    camera3(vec3 from, vec3 at, vec3 up, float vfov, float aspect, float aperture, float focus_dist, float t0, float t1)
    {
        float theta = vfov / 180.0f * 3.141592654;
        float half_height = tan(theta / 2);
        float height = half_height * 2;
        float width = height * aspect;

        this->w = unit_vector(from - at);
        this->u = unit_vector(cross(up, w));
        this->v = unit_vector(cross(w, u));
        
        this->origin = from;
        this->horizontal = u * focus_dist * width;
        this->vertical = v * focus_dist * height;

        this->lower_left_corner = this->origin - horizontal / 2 - vertical/2 - focus_dist * w;
        this->lens_radius = aperture / 2;

        this->time0 = t0;
        this->time1 = t1;
    }

    ray3 get_ray(float s, float t)
    {
        vec3 rd = lens_radius * random_in_unit_disk();
        vec3 offset = u * rd.x() + v * rd.y();
        float time = time0 + (time1 - time0) * random();

        return ray3
        (
            origin + offset, 
            lower_left_corner + horizontal * s + vertical * t - origin - offset, time
        );
    }

    vec3 w;
    vec3 u;
    vec3 v;
    vec3 origin;
    vec3 lower_left_corner;
    vec3 horizontal;
    vec3 vertical;

    // focus blur
    float lens_radius;

    // shutter speed
    float time0;
    float time1;
};

#endif