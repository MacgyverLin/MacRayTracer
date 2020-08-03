#ifndef _TRACE_RECORD_H_
#define _TRACE_RECORD_H_

#include "vec3.h"

class material;

class trace_record
{
public:
	trace_record()
	{
	}

	friend inline trace_record operator * (const mat4& mat, const trace_record& rec)
	{
		trace_record result;

		result.t = rec.t;
		result.position = mat.mul_position(rec.position);
		result.normal = mat.mul_direction(rec.normal);
		result.mat = rec.mat;
		result.texcoord = rec.texcoord;

		return result;
	}

	float t;
	vec3 position;
	vec3 normal;
	shared_ptr<material> mat;
	vec3 texcoord;
};

#endif