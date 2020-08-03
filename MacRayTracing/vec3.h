#ifndef _VEC3_h_
#define _VEC3_h_

#include "util.h"
#include "math.h"

#define VEC3_ELEMENT_COUNT (3)

class vec3
{
public:
	vec3()
	{
	}

	vec3(float e0, float e1, float e2)
	{
		e[0] = e0;
		e[1] = e1;
		e[2] = e2;
	}

	vec3(const vec3& other)
	{
		memcpy(e, other.e, VEC3_ELEMENT_COUNT * sizeof(float));
	}

	vec3& operator = (const vec3& other)
	{
		memcpy(e, other.e, VEC3_ELEMENT_COUNT * sizeof(float));

		return *this;
	}

	inline float x() const { return e[0]; }
	inline float y() const { return e[1]; }
	inline float z() const { return e[2]; }
	inline float operator[] (int i) const { return e[i];  }
	inline float& operator[] (int i) { return e[i]; }

	inline vec3 operator+() const { return *this; }

	inline vec3 operator-() const { return vec3(-e[0], -e[1], -e[2]); }

	inline vec3& operator += (const vec3& other)
	{
		e[0] += other.e[0];
		e[1] += other.e[1];
		e[2] += other.e[2];
		return *this;
	}

	inline vec3& operator -= (const vec3& other)
	{
		e[0] -= other.e[0];
		e[1] -= other.e[1];
		e[2] -= other.e[2];
		return *this;
	}

	inline vec3& operator *= (const vec3& other)
	{
		e[0] *= other.e[0];
		e[1] *= other.e[1];
		e[2] *= other.e[2];
		return *this;
	}

	inline vec3& operator /= (const vec3& other)
	{
		e[0] /= other.e[0];
		e[1] /= other.e[1];
		e[2] /= other.e[2];
		return *this;
	}

	inline vec3& operator *= (const float t)
	{
		e[0] *= t;
		e[1] *= t;
		e[2] *= t;
		return *this;
	}

	inline vec3& operator /= (const float t)
	{
		e[0] /= t;
		e[1] /= t;
		e[2] /= t;
		return *this;
	}

	friend inline vec3 operator + (const vec3& v1, const vec3& v2)
	{
		return vec3(v1.e[0] + v2.e[0], v1.e[1] + v2.e[1], v1.e[2] + v2.e[2]);
	}

	friend inline vec3 operator - (const vec3& v1, const vec3& v2)
	{
		return vec3(v1.e[0] - v2.e[0], v1.e[1] - v2.e[1], v1.e[2] - v2.e[2]);
	}

	friend inline vec3 operator * (const vec3& v1, const vec3& v2)
	{
		return vec3(v1.e[0] * v2.e[0], v1.e[1] * v2.e[1], v1.e[2] * v2.e[2]);
	}

	friend inline vec3 operator / (const vec3& v1, const vec3& v2)
	{
		return vec3(v1.e[0] / v2.e[0], v1.e[1] / v2.e[1], v1.e[2] / v2.e[2]);
	}

	friend inline vec3 operator * (const vec3& v1, const float& t)
	{
		return vec3(v1.e[0] * t, v1.e[1] * t, v1.e[2] * t);
	}

	friend inline vec3 operator / (const vec3& v1, const float& t)
	{
		return vec3(v1.e[0] / t, v1.e[1] / t, v1.e[2] / t);
	}

	friend inline vec3 operator * (const float& t, const vec3& v1)
	{
		return vec3(v1.e[0] * t, v1.e[1] * t, v1.e[2] * t);
	}

	inline void make_zero()
	{
		for (int i = 0; i < VEC3_ELEMENT_COUNT; i++)
			e[i] = 0;
	}

	inline void make_unitx()
	{
		e[0] = 1;
		e[1] = 0;
		e[2] = 0;
	}

	inline void make_unity()
	{
		e[0] = 0;
		e[1] = 1;
		e[2] = 0;
	}

	inline void make_unitz()
	{
		e[0] = 0;
		e[1] = 0;
		e[2] = 1;
	}

	inline float length() const 
	{ 
		return sqrt(square_length()); 
	}

	inline float square_length() const 
	{ 
		return e[0] * e[0] + e[1] * e[1] + e[2] * e[2]; 
	}

	inline void make_unit_vector()
	{
		*this = *this / length();
	}

	friend inline float dot(const vec3& v1, const vec3& v2)
	{
		return v1.e[0] * v2.e[0] + v1.e[1] * v2.e[1] + v1.e[2] * v2.e[2];
	}

	friend inline vec3 cross(const vec3& v1, const vec3& v2)
	{
		return vec3
		(
			(v1.e[1] * v2.e[2] - v1.e[2] * v2.e[1]),
			-(v1.e[0] * v2.e[2] - v1.e[2] * v2.e[0]),
			(v1.e[0] * v2.e[1] - v1.e[1] * v2.e[0])
		);
	}

	friend inline vec3 unit_vector(vec3 v)
	{
		float l = v.length();
		return v / l;
	}

	float e[3];
};

#endif