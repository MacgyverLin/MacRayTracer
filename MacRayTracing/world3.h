#ifndef _WORLD3_h_
#define _WORLD3_h_

#include "util.h"
#include "object3.h"

int comparebyx(const void* o0, const void* o1)
{
	aabb3 box0;
	aabb3 box1;

	(*((shared_ptr<traceable3>*)o0))->bounding_box(box0);
	(*((shared_ptr<traceable3>*)o1))->bounding_box(box1);

	return (box0.min().x() - box1.min().x()) < 0.0 ? -1 : 1;
}

int comparebyy(const void* o0, const void* o1)
{
	aabb3 box0;
	aabb3 box1;

	(*((shared_ptr<traceable3>*)o0))->bounding_box(box0);
	(*((shared_ptr<traceable3>*)o1))->bounding_box(box1);

	return (box0.min().y() - box1.min().y()) < 0.0 ? -1 : 1;
}

int comparebyz(const void* o0, const void* o1)
{
	aabb3 box0;
	aabb3 box1;

	(*((shared_ptr<traceable3>*)o0))->bounding_box(box0);
	(*((shared_ptr<traceable3>*)o1))->bounding_box(box1);

	return (box0.min().z() - box1.min().z()) < 0.0 ? -1 : 1;
}

void shuffle(vector<shared_ptr<traceable3>>& traceables_)
{
	for (int k = 0; k < 100; k++)
	{
		int i = random() * traceables_.size();
		int j = random() * traceables_.size();
		swap(traceables_[i], traceables_[j]);
	}
}

class bvhworld3 : public traceable3
{
public:
	bvhworld3()
		: traceable3(10000)
	{
	}

	static int getid()
	{
		static int i = 0;

		return i++;
	}

	bvhworld3(vector<shared_ptr<traceable3>>& traceables_, int start = -1, int n = -1)
		: traceable3(10000)
	{
		if (start < 0)
			start = 0;
		if (n < 0)
			n = traceables_.size();
		
		int axis = random() * 3;
		if (axis == 0)
			qsort(&traceables_[start], n, sizeof(traceables_[start]), comparebyx);
		else if (axis == 1)
			qsort(&traceables_[start], n, sizeof(traceables_[start]), comparebyy);
		else
			qsort(&traceables_[start], n, sizeof(traceables_[start]), comparebyz);
		
		/*
		printf("--------------------------------\n");
		shuffle(traceables_);
		for (int i = 0; i < traceables_.size(); i++)
		{
			aabb3 box;
			traceables_[i]->bounding_box(box);
			printf("%3.4f\n", box.min().x());
		}

		printf("--------------------------------\n");
		qsort(&traceables_[start], n, sizeof(traceables_[start]), comparebyx);
		for (int i = 0; i < traceables_.size(); i++)
		{
			aabb3 box;
			traceables_[i]->bounding_box(box);
			printf("%3.4f\n", box.min().x());
		}
		*/
		
		if (n==1)
		{
			left = right = traceables_[start];
		}
		else if (n == 2)
		{
			left = traceables_[start + 0];
			right = traceables_[start + 1];
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
			printf("no bounding box in bvhworld3 constructor");
		}

		box = box_left + box_right;
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

	virtual bool bounding_box(aabb3& b) const
	{
		b = box;

		return true;
	}

	virtual void print(int tab)
	{
		traceable3::print(tab);

		left->print(tab + 1);
		right->print(tab + 1);
	}

	shared_ptr<traceable3> left;
	shared_ptr<traceable3> right;
	aabb3 box;
};

class simpleworld3 : public traceable3
{
public:
	simpleworld3(const vector<shared_ptr<traceable3>>& traceables_)
		: traceable3(100000)
		, traceables(traceables_)
	{
	}

	~simpleworld3()
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

// typedef bvhworld3 world3;
typedef simpleworld3 world3;

#endif