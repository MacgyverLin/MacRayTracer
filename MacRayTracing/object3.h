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

	virtual bool trace(const ray3& r, float t_min, float t_max, trace_record& rec) const = 0;
	virtual bool bounding_box(float t0, float t1, aabb3& box) const = 0;
};

class transform_node : public traceable3
{
public:
	transform_node(shared_ptr<traceable3> tracable_, const mat4& mat_)
	: traceable3()
	{
		this->mat = mat_;
		this->invmat = mat_.inverse();

		this->tracable = tracable_;
	}

public:
	virtual bool trace(const ray3& r, float t_min, float t_max, trace_record& rec) const
	{
		ray3 tr = invmat * r;
		if (tracable->trace(tr, t_min, t_max, rec))
		{
			rec = mat * rec;
			return true;
		}
		else
		{
			return false;
		}
	}

	virtual bool bounding_box(float t0, float t1, aabb3& box) const
	{
		return tracable->bounding_box(t0, t1, box);
	}

	shared_ptr<traceable3> tracable;
	mat4 mat;
	mat4 invmat;
};

class flip_normal : public traceable3
{
public:
	flip_normal(shared_ptr<traceable3> tracable)
		: traceable3()
	{
		this->tracable = tracable;
	}

public:
	virtual bool trace(const ray3& r, float t_min, float t_max, trace_record& rec) const
	{
		if (tracable->trace(r, t_min, t_max, rec))
		{
			rec.normal = -rec.normal;
			return true;
		}
		else
		{
			return false;
		}
	}

	virtual bool bounding_box(float t0, float t1, aabb3& box) const
	{
		return tracable->bounding_box(t0, t1, box);
	}

	shared_ptr<traceable3> tracable;
};

#endif