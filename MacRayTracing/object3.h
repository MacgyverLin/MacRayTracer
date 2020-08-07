#ifndef _OBJECT3_h_
#define _OBJECT3_h_

#include "vec3.h"
#include "ray3.h"
#include "material.h"
#include "aabb3.h"

class traceable3
{
public:
	traceable3(int base)
	{
		static int ii = 0;

		id = base + ii++;
	}

	virtual bool trace(const ray3& r, float t_min, float t_max, trace_record& rec) const = 0;
	virtual bool bounding_box(aabb3& box) const = 0;
	int id;

	virtual void printTab(int tab)
	{
		for(int i=0; i<tab; i++)
			printf("+----");
	}

	virtual void print(int tab)
	{
		printTab(tab);
		printf("%d\n", id);
	}
};

class transform_node : public traceable3
{
public:
	transform_node(shared_ptr<traceable3> tracable_, const mat4& mat_)
	: traceable3(70000)
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

	virtual bool bounding_box(aabb3& box) const
	{
		return tracable->bounding_box(box);
	}

	shared_ptr<traceable3> tracable;
	mat4 mat;
	mat4 invmat;
};

class flip_normal : public traceable3
{
public:
	flip_normal(shared_ptr<traceable3> tracable)
		: traceable3(80000)
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

	virtual bool bounding_box(aabb3& box) const
	{
		return tracable->bounding_box(box);
	}

	shared_ptr<traceable3> tracable;
};

class const_medium : public traceable3
{
public:
	const_medium(shared_ptr<traceable3> tracable_, float density_, shared_ptr<texture> texture_)
		: traceable3(90000)
	{
		this->boundary = tracable_;
		this->density = density_;
		this->mat = make_shared<isotropic>(texture_);
	}

public:
	virtual bool trace(const ray3& r, float t_min, float t_max, trace_record& rec) const
	{
		bool debug = (random() < 0.0001);
		// debug = false;
		trace_record rec1, rec2;
		if (boundary->trace(r, -FLT_MAX, FLT_MAX, rec1))
		{
			if (boundary->trace(r, rec1.t+0.00001, FLT_MAX, rec2))
			{
				if (rec1.t < t_min)
				{
					rec1.t = t_min;
				}
				if (rec2.t > t_max)
				{
					rec2.t = t_max;
				}
				if (rec1.t >= rec2.t)
					return false;
				if (rec1.t < 0)
					rec1.t = 0;
				float distance_inside_boundary = (rec2.t - rec1.t) * r.direction().length();
				float hit_distance = -(1.0 / density) * log(random());
				if (hit_distance < distance_inside_boundary)
				{
					rec.t = rec1.t + hit_distance / r.direction().length();
					rec.position = r.point_at_parameter(rec.t);
					rec.normal = unit_vector(vec3(random(), random(), random()));
					rec.mat = mat;
					return true;
				}
			}
		}
		return false;
	}

	virtual bool bounding_box(aabb3& box) const
	{
		return boundary->bounding_box(box);
	}

	shared_ptr<traceable3> boundary;
	float density;
	shared_ptr<material> mat;
};

#endif