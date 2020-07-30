#ifndef _WORLD3_h_
#define _WORLD3_h_

#include "object3.h"

class world3 : public traceable3
{
public:
	world3()
	{
	}

	~world3()
	{
		for (auto& o : objects)
		{
			delete o;
		}

		objects.clear();
	}

	void add(object3* o)
	{
		objects.push_back(o);
	}

	virtual bool trace(const ray3& r, float t_min, float t_max, trace_record& rec) const
	{
		float closest_t = t_max;
		bool hitsomething = false;
		trace_record temp_rec;

		for (auto& o : objects)
		{
			if (o->trace(r, t_min, closest_t, temp_rec))
			{
				hitsomething = true;
				closest_t = temp_rec.t;
				rec = temp_rec;
			}
		}

		return hitsomething;
	}

	std::vector<object3*> objects;
};

#endif