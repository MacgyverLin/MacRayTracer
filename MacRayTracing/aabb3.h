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

	bool trace(const ray3& r, float tmin, float tmax) const
	{
		for (int a = 0; a < 3; a++) 
		{
			auto t0 = fmin((_min[a] - r.origin()[a]) / r.direction()[a],
				(_max[a] - r.origin()[a]) / r.direction()[a]);
			auto t1 = fmax((_min[a] - r.origin()[a]) / r.direction()[a],
				(_max[a] - r.origin()[a]) / r.direction()[a]);
			tmin = fmax(t0, tmin);
			tmax = fmin(t1, tmax);
			if (tmax <= tmin)
				return false;
		}
		return true;
	}	

	friend aabb3 operator + (const aabb3& box0, const aabb3& box1)
	{
		vec3 small(fmin(box0.min().x(), box1.min().x()),
			fmin(box0.min().y(), box1.min().y()),
			fmin(box0.min().z(), box1.min().z()));

		vec3 big(fmax(box0.max().x(), box1.max().x()),
			fmax(box0.max().y(), box1.max().y()),
			fmax(box0.max().z(), box1.max().z()));

		return aabb3(small, big);
	}

	vec3 _min;
	vec3 _max;
};

#endif