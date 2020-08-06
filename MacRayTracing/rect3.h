#ifndef _rect3_h_
#define _rect3_h_

#include "object3.h"
#include "ray3.h"
#include "aabb3.h"
#include "util.h"

class xy_rect : public traceable3
{
public:
	xy_rect(float x0_, float x1_, float y0_, float y1_, float k_, shared_ptr<material> mat_)
	: traceable3()
	{
		this->x0 = x0_;
		this->x1 = x1_;
		this->y0 = y0_;
		this->y1 = y1_;
		this->k = k_;

		mat = mat_;
	}

	virtual bool trace(const ray3& r, float t_min, float t_max, trace_record& rec) const
	{
		float t = (k - r.origin().z()) / r.direction().z();
		if (t<t_min || t> t_max)
			return false;

		vec3 p = r.point_at_parameter(t);
		if (p.x() < x0 || p.x() > x1 || p.y() < y0 || p.y() > y1)
			return false;

		rec.position = p;
		rec.normal = vec3(0, 0, 1);
		rec.mat = mat;
		rec.texcoord[0] = (p.x() - x0) / (x1 - x0);
		rec.texcoord[1] = (p.y() - y0) / (y1 - y0);
		rec.texcoord[2] = 1;
		rec.t = t;

		return true;
	}

	virtual bool bounding_box(aabb3& box) const
	{
		return true;
	}

	float x0;
	float x1;
	float y0;
	float y1;
	float k;

	shared_ptr<material> mat;
};

class yz_rect : public traceable3
{
public:
	yz_rect(float y0_, float y1_, float z0_, float z1_, float k_, shared_ptr<material> mat_)
		: traceable3()
	{
		this->y0 = y0_;
		this->y1 = y1_;
		this->z0 = z0_;
		this->z1 = z1_;
		this->k = k_;

		mat = mat_;
	}

	virtual bool trace(const ray3& r, float t_min, float t_max, trace_record& rec) const
	{
		float t = (k - r.origin().x()) / r.direction().x();
		if (t<t_min || t> t_max)
			return false;

		vec3 p = r.point_at_parameter(t);
		if (p.y() < y0 || p.y() > y1 || p.z() < z0 || p.z() > z1)
			return false;

		rec.position = p;
		rec.normal = vec3(1, 0, 0);
		rec.mat = mat;
		rec.t = t;
		rec.texcoord[0] = (p.y() - y0) / (y1 - y0);
		rec.texcoord[1] = (p.z() - z0) / (z1 - z0);
		rec.texcoord[2] = 1;

		return true;
	}

	virtual bool bounding_box(aabb3& box) const
	{
		return true;
	}

	float y0;
	float y1;
	float z0;
	float z1;
	float k;

	shared_ptr<material> mat;
};

class xz_rect : public traceable3
{
public:
	xz_rect(float x0_, float x1_, float z0_, float z1_, float k_, shared_ptr<material> mat_)
		: traceable3()
	{
		this->x0 = x0_;
		this->x1 = x1_;
		this->z0 = z0_;
		this->z1 = z1_;
		this->k = k_;

		mat = mat_;
	}

	virtual bool trace(const ray3& r, float t_min, float t_max, trace_record& rec) const
	{
		float t = (k - r.origin().y()) / r.direction().y();
		if (t<t_min || t> t_max)
			return false;

		vec3 p = r.point_at_parameter(t);
		if (p.z() < z0 || p.z() > z1 || p.x() < x0 || p.x() > x1)
			return false;

		rec.position = p;
		rec.normal = vec3(0, 1, 0);
		rec.mat = mat;
		rec.t = t;
		rec.texcoord[0] = (p.z() - z0) / (z1 - z0);
		rec.texcoord[1] = (p.x() - x0) / (x1 - x0);
		rec.texcoord[2] = 1;

		return true;
	}

	virtual bool bounding_box(aabb3& box) const
	{
		return true;
	}

	float x0;
	float x1;
	float z0;
	float z1;
	float k;

	shared_ptr<material> mat;
};


#endif