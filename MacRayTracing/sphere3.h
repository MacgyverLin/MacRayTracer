#ifndef _SPHERE3_h_
#define _SPHERE3_h_

#include "object3.h"
#include "ray3.h"
#include "aabb3.h"
#include "util.h"

class sphere3 : public object3
{
public:
	sphere3(const vec3& center, float radius, shared_ptr<material> mat, const vec3& velocity = vec3(0, 0, 0))
	: object3(mat)
	{
		this->center = center;
		this->radius = radius;
		this->velocity = velocity;
	}

	void getUV(const vec3& p, vec3& texcoord) const
	{
		float phi = atan2(p.z(), p.x());
		float theta = asin(p.y());

		texcoord[0] = 1 - (phi + ONE_PI) / (TWO_PI);
		texcoord[1] = (theta + HALF_PI) / (ONE_PI);
		texcoord[2] = 1.0f;
	}

	virtual bool trace(const ray3& r, float t_min, float t_max, trace_record& rec) const
	{
		vec3 centert = getCenter(r.time());
		vec3 oc = r.origin() - centert;
		float a = dot(r.direction(), r.direction());
		float b = dot(oc, r.direction());
		float c = dot(oc, oc) - radius * radius;
		float discriminant = b * b - a * c;

		if (discriminant > 0)
		{
			float temp = (-b - sqrt(discriminant)) / a;
			if (temp< t_max && temp> t_min)
			{
				rec.t = temp;
				rec.position = r.point_at_parameter(rec.t);
				rec.normal = (rec.position - centert) / radius;
				rec.mat = mat;
				getUV((rec.position - centert) / radius, rec.texcoord);
				return true;
			}

			temp = (-b + sqrt(discriminant)) / a;
			if (temp< t_max && temp> t_min)
			{
				rec.t = temp;
				rec.position = r.point_at_parameter(rec.t);
				rec.normal = (rec.position - centert) / radius;
				rec.mat = mat;
				getUV((rec.position - centert) / radius, rec.texcoord);
				return true;
			}
		}

		return false;
	}

	virtual bool bounding_box_static(float t, aabb3& box) const
	{
		vec3 center_t = getCenter(t);
		box = aabb3(center_t - vec3(radius, radius, radius), center_t + vec3(radius, radius, radius));

		return true;
	}

	virtual bool bounding_box_dynamic(float t0, float t1, aabb3& box) const
	{
		aabb3 box0;
		aabb3 box1;
		bounding_box_static(t0, box0);
		bounding_box_static(t1, box1);

		vec3 small(ffmin(box0.min()[0], box1.min()[0]),
			ffmin(box0.min()[1], box1.min()[1]),
			ffmin(box0.min()[2], box1.min()[2]));
		vec3 big(ffmax(box0.max()[0], box1.max()[0]),
			ffmax(box0.max()[1], box1.max()[1]),
			ffmax(box0.max()[2], box1.max()[2]));

		box = aabb3(small, big);
		return true;
	}

	virtual bool bounding_box(float t0, float t1, aabb3& box) const
	{
		return bounding_box_dynamic(t0, t1, box);
	}

	vec3 getCenter(float time) const
	{
		return center + velocity * time;
	}

	vec3 center;
	float radius;
	vec3 velocity;
};

#endif