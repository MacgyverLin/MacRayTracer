#ifndef _SPHERE3_h_
#define _SPHERE3_h_

#include "object3.h"
#include "ray3.h"

class sphere3 : public object3
{
public:
	sphere3(material* mat)
	: object3 (mat)
	{
	}

	sphere3(const vec3& center, float radius, material* mat)
	: object3(mat)
	{
		this->center = center;
		this->radius = radius;
	}

	virtual bool trace(const ray3& r, float t_min, float t_max, trace_record& rec) const
	{
		vec3 oc = r.origin() - center;
		float a = dot(r.direction(), r.direction());
		float b = dot(oc, r.direction());
		float c = dot(oc, oc) - radius * radius;
		float discriminant = b * b - a * c;

		if (discriminant > 0)
		{
			float temp = (-b - sqrt(discriminant))/a;
			if (temp< t_max && temp> t_min)
			{
				rec.t = temp;
				rec.position = r.point_at_parameter(rec.t);
				rec.normal = (rec.position - center) / radius;
				rec.mat = mat;
				return true;
			}
			
			temp = (-b + sqrt(discriminant))/a;
			if (temp< t_max && temp> t_min)
			{
				rec.t = temp;
				rec.position = r.point_at_parameter(rec.t);
				rec.normal = (rec.position - center) / radius;
				rec.mat = mat;
				return true;
			}
		}
		
		return false;
	}

	vec3 center;
	float radius;
public:
};

#endif