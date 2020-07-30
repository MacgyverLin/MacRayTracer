#ifndef _RAY3_H_
#define _RAY3_H_

#include "vec3.h"

class ray3
{
public:
	ray3()
	{
	}

	ray3(const vec3& o, const vec3& d)
	{
		this->o = o;
		this->d = d;
	}

	const vec3& origin() const
	{
		return o;
	}

	const vec3& direction() const
	{
		return d;
	}

	vec3 point_at_parameter(float t) const
	{
		return o + t * d;
	}

	vec3 o;
	vec3 d;
};

#endif