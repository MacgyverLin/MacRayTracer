#ifndef _box3_h_
#define _box3_h_

#include "rect3.h"
#include "rect3.h"

class box3 : public traceable3
{
public:
	box3(const vec3& p0_, const vec3& p1_, shared_ptr<material> mat_)
		: traceable3()
	{
		p0 = p0_;
		p1 = p1_;

		faces.resize(6);
		faces[0] = new xy_rect(p0.x(), p1.x(), p0.y(), p1.y(), p1.z(), mat_);
		faces[1] = new flip_normal(make_shared<xy_rect>(p0.x(), p1.x(), p0.y(), p1.y(), p0.z(), mat_));
		faces[2] = new xz_rect(p0.x(), p1.x(), p0.z(), p1.z(), p1.y(), mat_);
		faces[3] = new flip_normal(make_shared<xz_rect>(p0.x(), p1.x(), p0.z(), p1.z(), p0.y(), mat_));
		faces[4] = new yz_rect(p0.y(), p1.y(), p0.z(), p1.z(), p1.x(), mat_);
		faces[5] = new flip_normal(make_shared<yz_rect>(p0.y(), p1.y(), p0.z(), p1.z(), p0.x(), mat_));
	}

	~box3()
	{
		for (int i = 0; i < 6; i++)
		{
			delete faces[i];
			faces[i] = nullptr;
		}
	}

	virtual bool trace(const ray3& r, float t_min, float t_max, trace_record& rec) const
	{
		float closest_t = t_max;
		bool hitsomething = false;
		trace_record temp_rec;

		for (auto& face : faces)
		{
			if (face->trace(r, t_min, closest_t, temp_rec))
			{
				hitsomething = true;
				closest_t = temp_rec.t;
				rec = temp_rec;
			}
		}

		return hitsomething;
	}

	virtual bool bounding_box(float t0, float t1, aabb3& box) const
	{
		return true;
	}

	vec3 p0;
	vec3 p1;

	vector<traceable3*> faces;
};

#endif