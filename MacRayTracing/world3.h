#ifndef _WORLD3_h_
#define _WORLD3_h_

#include "util.h"
#include "object3.h"

/*
bool comparebyx(object3* o0, object3 *o1)
{ 
	aabb3 box0;
	aabb3 box1;
	
	o0->bounding_box(0, 0, box0);
	o1->bounding_box(0, 0, box1);

	return box0.min().x() - box0.min().x();
}

bool comparebyy(object3* o0, object3* o1)
{
	aabb3 box0;
	aabb3 box1;

	o0->bounding_box(0, 0, box0);
	o1->bounding_box(0, 0, box1);

	return box0.min().y() - box0.min().y();
}

bool comparebyz(object3* o0, object3* o1)
{
	aabb3 box0;
	aabb3 box1;

	o0->bounding_box(0, 0, box0);
	o1->bounding_box(0, 0, box1);

	return box0.min().z() - box0.min().z();
}

class bvhworld : public traceable3
{
public:
	bvhworld(const vector<shared_ptr<object3>>& objects, int s, int n)
	{
		int axis = int(3 * random());
		if (axis == 0)
		{
			std::sort(objects.begin(), objects.end(), comparebyx);
		}
		else if (axis == 1)
		{
			std::sort(objects.begin(), objects.end(), comparebyy);
		}
		else
		{
			std::sort(objects.begin(), objects.end(), comparebyz);
		}

		if (n == 1)
		{
			left = right = objects[s];
		}
		else if (n == 2)
		{
			left = objects[s];
			right = objects[s+1];
		}
		else
		{
			left = make_shared<bvhworld>(objects, s, n / 2);
			right = make_shared<bvhworld>(objects, s + n / 2, n - n / 2);
		}

		aabb3 box_left;
		aabb3 box_right;

		left->bounding_box(box_left
	}

	~bvhworld()
	{
	}

	virtual bool trace(const ray3& r, float t_min, float t_max, trace_record& record) const
	{
		if (bound.trace(r, t_min, t_max))
		{
			trace_record record_left;
			trace_record record_right;
			bool hit_left = left->trace(r, t_min, t_max, record_left);
			bool hit_right = right->trace(r, t_min, t_max, record_right);
			if (hit_left && hit_right)
			{
				if (record_left.t < record_right.t)
				{
					record = record_left;
				}
				else
				{
					record = record_right;
				}

				return true;
			}
			else if (hit_left)
			{
				record = record_left;
				return true;
			}
			else if (hit_right)
			{
				record = record_right;
				return true;
			}
			else
				return false;
		}
		else
		{
			return false;
		}
	}

	virtual bool bounding_box(float t0, float t1, aabb3& box) const
	{
		box = bound;
		return true;
	}

	shared_ptr<traceable3> left;
	shared_ptr<traceable3> right;
	aabb3 bound;
};
*/

bool comparebyx(shared_ptr<traceable3>* o0, shared_ptr<traceable3>* o1)
{
	aabb3 box0;
	aabb3 box1;

	(*o0)->bounding_box(box0);
	(*o1)->bounding_box(box1);

	return box0.min().x() - box0.min().x();
}

bool comparebyy(shared_ptr<traceable3>* o0, shared_ptr<traceable3>* o1)
{
	aabb3 box0;
	aabb3 box1;

	(*o0)->bounding_box(box0);
	(*o1)->bounding_box(box1);

	return box0.min().y() - box0.min().y();
}

bool comparebyz(shared_ptr<traceable3>* o0, shared_ptr<traceable3>* o1)
{
	aabb3 box0;
	aabb3 box1;

	(*o0)->bounding_box(box0);
	(*o1)->bounding_box(box1);

	return box0.min().z() - box0.min().z();
}

class bvhworld3 : public traceable3
{
public:
	bvhworld3(const vector<shared_ptr<traceable3>>& traceables_, int start, int n)
	{
		int axis = 0;
		
		if (n==1)
		{
			left = right = traceables_[0];
		}
		else if (n == 2)
		{
			left = traceables_[0];
			right = traceables_[1];
		}
		else
		{
			left = make_shared<bvhworld3>(traceables_, start, n / 2);
			right = make_shared<bvhworld3>(traceables_, start + n / 2, n - n / 2);
		}

		aabb3 box_left;
		aabb3 box_right;
		if (!left->bounding_box(box_left) || !right->bounding_box(box_right))
		{
			box = box_left + box_right;
		}
	}

	~bvhworld3()
	{
	}

	virtual bool trace(const ray3& r, float t_min, float t_max, trace_record& rec) const
	{
		if (box.trace(r, t_min, t_max))
		{
			trace_record left_rec;
			trace_record right_rec;
			bool hit_left = left->trace(r, t_min, t_max, left_rec);
			bool hit_right = right->trace(r, t_min, t_max, right_rec);

			if (hit_left && hit_right)
			{
				if (left_rec.t < right_rec.t)
				{
					rec = left_rec;
				}
				else
				{
					rec = right_rec;
				}
				return true;
			}
			else if (hit_left)
			{
				rec = left_rec;
				return true;
			}
			else if (hit_right)
			{
				rec = right_rec;
				return true;
			}
			else
				return false;
		}
		else
		{
			return false;
		}
	}

	virtual bool bounding_box(aabb3& box) const
	{
		return true;
	}

	shared_ptr<traceable3> left;
	shared_ptr<traceable3> right;
	aabb3 box;
};

class world3 : public traceable3
{
public:
	world3(const vector<shared_ptr<traceable3>>& traceables_)
		: traceables(traceables_)
	{
	}

	~world3()
	{
	}

	void clear()
	{
		traceables.clear();
	}

	virtual bool trace(const ray3& r, float t_min, float t_max, trace_record& rec) const
	{
		float closest_t = t_max;
		bool hitsomething = false;
		trace_record temp_rec;

		for (auto& tracable : traceables)
		{
			if (tracable->trace(r, t_min, closest_t, temp_rec))
			{
				hitsomething = true;
				closest_t = temp_rec.t;
				rec = temp_rec;
			}
		}

		return hitsomething;
	}

	virtual bool bounding_box(aabb3& box) const
	{
		return true;
	}

	vector<shared_ptr<traceable3>> traceables;
};

#endif