#ifndef _RAY3_H_
#define _RAY3_H_

#include "vec3.h"

class ray3
{
public:
	ray3()
	{
	}

	ray3(const vec3& o, const vec3& d, float t)
	{
		this->o = o;
		this->d = d;
		this->t = t;
	}

	const vec3& origin() const
	{
		return this->o;
	}

	const vec3& direction() const
	{
		return this->d;
	}

	float time() const
	{
		return this->t;
	}

	vec3 point_at_parameter(float t) const
	{
		return this->o + this->d * t;
	}

	vec3 o;
	vec3 d;
	float t;
};

#endif