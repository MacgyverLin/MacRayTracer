#ifndef _AABB3_h_
#define _AABB3_h_

#include <algorithm>
#include "ray3.h"

class aabb3
{
public:
	aabb3()
	{
	}

	aabb3(const vec3& min, const vec3& max)
	{
		this->_min = min;
		this->_max = max;
	}

	vec3 min()
	{
		return _min;
	}

	vec3 max()
	{
		return _max;
	}

	bool trace2(const ray3& r, float tmin, float tmax) const
	{
		for (int i = 0; i < 3; i++)
		{
			float invD = 1.0f / r.direction()[i];
			float t0 = _min[i] - r.origin()[i] * invD;
			float t1 = _max[i] - r.origin()[i] * invD;
			if (invD < 0)
				swap(t0, t1);

			tmin = ffmax(t0, tmin);
			tmax = ffmin(t1, tmax);
			if (tmax < tmin)
				return false;
		}
		return true;
	}

	bool trace(const ray3& r, float tmin, float tmax) const
	{
		for (int i = 0; i < 3; i++)
		{
			float a = _min[i] - r.origin()[i] / r.direction()[i];
			float b = _max[i] - r.origin()[i] / r.direction()[i];
			float t0 = ffmin(a, b);
			float t1 = ffmax(a, b);

			tmin = ffmax(t0, tmin);
			tmax = ffmin(t1, tmax);
			if (tmax < tmin)
				return false;
		}
		return true;
	}

	vec3 _min;
	vec3 _max;
};

#endif