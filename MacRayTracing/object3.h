#ifndef _OBJECT3_h_
#define _OBJECT3_h_

#include "vec3.h"
#include "ray3.h"
#include "material.h"
#include "aabb3.h"

class traceable3
{
public:
	traceable3()
	{
	}

	virtual bool trace(const ray3& r, float t_min, float tmax, trace_record& rec) const = 0;
	virtual bool bounding_box(float t0, float t1, aabb3& box) const = 0;
};

class object3 : public traceable3
{
public:
	object3(shared_ptr<material> mat)
	: traceable3()
	{
		this->mat = mat;
	}

public:
	virtual bool trace(const ray3& r, float t_min, float tmax, trace_record& rec) const = 0;
	virtual bool bounding_box(float t0, float t1, aabb3& box) const = 0;

	shared_ptr<material> mat;
};

#endif