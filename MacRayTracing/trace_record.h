#ifndef _TRACE_RECORD_H_
#define _TRACE_RECORD_H_

#include "vec3.h"

class material;

class trace_record
{
public:
	float t;
	vec3 position;
	vec3 normal;
	shared_ptr<material> mat;
	vec3 texcoord;
};

#endif