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

	const vec3& min() const
	{
		return _min;
	}

	const vec3& max() const
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
			float a = (_min[i] - r.origin()[i]) / r.direction()[i];
			float b = (_max[i] - r.origin()[i]) / r.direction()[i];
			float t0 = ffmin(a, b);
			float t1 = ffmax(a, b);

			tmin = ffmax(t0, tmin);
			tmax = ffmin(t1, tmax);
			if (tmax < tmin)
				return false;
		}
		return true;
	}

	friend aabb3 operator + (const aabb3& aabb0, const aabb3& aabb1)
	{
		vec3 min
		( 
			ffmin(aabb0.min().x(), aabb1.max().x()),
			ffmin(aabb0.min().y(), aabb1.max().y()),
			ffmin(aabb0.min().z(), aabb1.max().z())
		);

		vec3 max
		(
			ffmax(aabb0.min().x(), aabb1.max().x()),
			ffmax(aabb0.min().y(), aabb1.max().y()),
			ffmax(aabb0.min().z(), aabb1.max().z())
		);

		return aabb3(min, max);
	}

	vec3 _min;
	vec3 _max;
};

#endif