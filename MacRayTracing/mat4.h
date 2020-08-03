#ifndef _MAT4_h_
#define _MAT4_h_

#include "util.h"
#include "math.h"

#define MAT4_ELEMENT_ROW		(4)
#define MAT4_ELEMENT_COLUMN		(4)
#define MAT4_ELEMENT_COUNT		(MAT4_ELEMENT_ROW * MAT4_ELEMENT_COLUMN)

class mat4
{
public:
	mat4()
	{
	}

	mat4(float* e_)
	{
		memcpy(e, e_, MAT4_ELEMENT_COUNT * sizeof(float));
	}

	mat4(float e00, float e01, float e02, float e03, 
		 float e10, float e11, float e12, float e13,
		 float e20, float e21, float e22, float e23,
		 float e30, float e31, float e32, float e33)
	{
		e[ 0] = e00; e[ 1] = e01; e[ 2] = e02; e[ 3] = e03;
		e[ 4] = e10; e[ 5] = e11; e[ 6] = e12; e[ 7] = e13;
		e[ 8] = e20; e[ 9] = e21; e[10] = e22; e[11] = e23;
		e[12] = e30; e[13] = e31; e[14] = e32; e[15] = e33;
	}

	mat4(const vec3& other)
	{
		memcpy(e, other.e, MAT4_ELEMENT_COUNT * sizeof(float));
	}

	mat4& operator = (const mat4& other)
	{
		memcpy(e, other.e, MAT4_ELEMENT_COUNT * sizeof(float));

		return *this;
	}

	inline float* operator[] (int row)
	{
		return &e[row * MAT4_ELEMENT_COLUMN];
	}

	inline const float* operator[] (int row) const
	{
		return &e[row * MAT4_ELEMENT_COLUMN];
	}

	inline mat4 operator+() const 
	{ 
		return *this; 
	}

	inline mat4 operator-() const 
	{ 
		mat4 m = *this;
		for (int i = 0; i < MAT4_ELEMENT_COUNT; i++)
			m.e[i] = -m.e[i];
	}

	inline mat4& operator += (const mat4& other)
	{
		for (int i = 0; i < MAT4_ELEMENT_COUNT; i++)
			e[i] += other.e[i];

		return *this;
	}

	inline mat4& operator -= (const mat4& other)
	{
		for (int i = 0; i < MAT4_ELEMENT_COUNT; i++)
			e[i] -= other.e[i];

		return *this;
	}

	inline mat4& operator *= (const mat4& m1)
	{
		mat4 m0 = *this;
		
		*this[0][0] = m0[0][0] * m1[0][0] + m0[0][1] * m1[1][0] + m0[0][2] * m1[2][0] + m0[0][3] *m1[3][0];
		*this[0][1] = m0[0][0] * m1[0][1] + m0[0][1] * m1[1][1] + m0[0][2] * m1[2][1] + m0[0][3] *m1[3][1];
		*this[0][2] = m0[0][0] * m1[0][2] + m0[0][1] * m1[1][2] + m0[0][2] * m1[2][2] + m0[0][3] *m1[3][2];
		*this[0][3] = m0[0][0] * m1[0][3] + m0[0][1] * m1[1][3] + m0[0][2] * m1[2][3] + m0[0][3] *m1[3][3];

		*this[1][0] = m0[1][0] * m1[0][0] + m0[1][1] * m1[1][0] + m0[1][2] * m1[2][0] + m0[1][3] *m1[3][0];
		*this[1][1] = m0[1][0] * m1[0][1] + m0[1][1] * m1[1][1] + m0[1][2] * m1[2][1] + m0[1][3] *m1[3][1];
		*this[1][2] = m0[1][0] * m1[0][2] + m0[1][1] * m1[1][2] + m0[1][2] * m1[2][2] + m0[1][3] *m1[3][2];
		*this[1][3] = m0[1][0] * m1[0][3] + m0[1][1] * m1[1][3] + m0[1][2] * m1[2][3] + m0[1][3] *m1[3][3];

		*this[2][0] = m0[2][0] * m1[0][0] + m0[2][1] * m1[1][0] + m0[2][2] * m1[2][0] + m0[2][3] *m1[3][0];
		*this[2][1] = m0[2][0] * m1[0][1] + m0[2][1] * m1[1][1] + m0[2][2] * m1[2][1] + m0[2][3] *m1[3][1];
		*this[2][2] = m0[2][0] * m1[0][2] + m0[2][1] * m1[1][2] + m0[2][2] * m1[2][2] + m0[2][3] *m1[3][2];
		*this[2][3] = m0[2][0] * m1[0][3] + m0[2][1] * m1[1][3] + m0[2][2] * m1[2][3] + m0[2][3] *m1[3][3];

		*this[3][0] = m0[3][0] * m1[0][0] + m0[3][1] * m1[1][0] + m0[3][2] * m1[2][0] + m0[3][3] *m1[3][0];
		*this[3][1] = m0[3][0] * m1[0][1] + m0[3][1] * m1[1][1] + m0[3][2] * m1[2][1] + m0[3][3] *m1[3][1];
		*this[3][2] = m0[3][0] * m1[0][2] + m0[3][1] * m1[1][2] + m0[3][2] * m1[2][2] + m0[3][3] *m1[3][2];
		*this[3][3] = m0[3][0] * m1[0][3] + m0[3][1] * m1[1][3] + m0[3][2] * m1[2][3] + m0[3][3] *m1[3][3];

		return *this;
	}

	inline mat4& operator *= (const float t)
	{
		for (int i = 0; i < MAT4_ELEMENT_COUNT; i++)
			e[i] *= t;

		return *this;
	}

	inline mat4& operator /= (const float t)
	{
		for (int i = 0; i < MAT4_ELEMENT_COUNT; i++)
			e[i] /= t;

		return *this;
	}

	friend inline mat4 operator + (const mat4& m0, const mat4& m1)
	{
		return mat4
		(
			m0.e[ 0] + m1.e[ 0], m0.e[ 1] + m1.e[ 1], m0.e[ 2] + m1.e[ 2], m0.e[ 3] + m1.e[ 3],
			m0.e[ 4] + m1.e[ 4], m0.e[ 5] + m1.e[ 5], m0.e[ 6] + m1.e[ 6], m0.e[ 7] + m1.e[ 7],
			m0.e[ 8] + m1.e[ 8], m0.e[ 9] + m1.e[ 9], m0.e[10] + m1.e[10], m0.e[11] + m1.e[11],
			m0.e[12] + m1.e[12], m0.e[13] + m1.e[13], m0.e[14] + m1.e[14], m0.e[15] + m1.e[15]
		);
	}

	friend inline mat4 operator - (const mat4& m0, const mat4& m1)
	{
		return mat4
		(
			m0.e[0] - m1.e[0], m0.e[1] - m1.e[1], m0.e[2] - m1.e[2], m0.e[3] - m1.e[3],
			m0.e[4] - m1.e[4], m0.e[5] - m1.e[5], m0.e[6] - m1.e[6], m0.e[7] - m1.e[7],
			m0.e[8] - m1.e[8], m0.e[9] - m1.e[9], m0.e[10] - m1.e[10], m0.e[11] - m1.e[11],
			m0.e[12] - m1.e[12], m0.e[13] - m1.e[13], m0.e[14] - m1.e[14], m0.e[15] - m1.e[15]
		);
	}

	friend inline mat4 operator * (const mat4& m0, const mat4& m1)
	{
		mat4 m;

		m[0][0] = m0[0][0] * m1[0][0] + m0[0][1] * m1[1][0] + m0[0][2] * m1[2][0] + m0[0][3] * m1[3][0];
		m[0][1] = m0[0][0] * m1[0][1] + m0[0][1] * m1[1][1] + m0[0][2] * m1[2][1] + m0[0][3] * m1[3][1];
		m[0][2] = m0[0][0] * m1[0][2] + m0[0][1] * m1[1][2] + m0[0][2] * m1[2][2] + m0[0][3] * m1[3][2];
		m[0][3] = m0[0][0] * m1[0][3] + m0[0][1] * m1[1][3] + m0[0][2] * m1[2][3] + m0[0][3] * m1[3][3];

		m[1][0] = m0[1][0] * m1[0][0] + m0[1][1] * m1[1][0] + m0[1][2] * m1[2][0] + m0[1][3] * m1[3][0];
		m[1][1] = m0[1][0] * m1[0][1] + m0[1][1] * m1[1][1] + m0[1][2] * m1[2][1] + m0[1][3] * m1[3][1];
		m[1][2] = m0[1][0] * m1[0][2] + m0[1][1] * m1[1][2] + m0[1][2] * m1[2][2] + m0[1][3] * m1[3][2];
		m[1][3] = m0[1][0] * m1[0][3] + m0[1][1] * m1[1][3] + m0[1][2] * m1[2][3] + m0[1][3] * m1[3][3];

		m[2][0] = m0[2][0] * m1[0][0] + m0[2][1] * m1[1][0] + m0[2][2] * m1[2][0] + m0[2][3] * m1[3][0];
		m[2][1] = m0[2][0] * m1[0][1] + m0[2][1] * m1[1][1] + m0[2][2] * m1[2][1] + m0[2][3] * m1[3][1];
		m[2][2] = m0[2][0] * m1[0][2] + m0[2][1] * m1[1][2] + m0[2][2] * m1[2][2] + m0[2][3] * m1[3][2];
		m[2][3] = m0[2][0] * m1[0][3] + m0[2][1] * m1[1][3] + m0[2][2] * m1[2][3] + m0[2][3] * m1[3][3];

		m[3][0] = m0[3][0] * m1[0][0] + m0[3][1] * m1[1][0] + m0[3][2] * m1[2][0] + m0[3][3] * m1[3][0];
		m[3][1] = m0[3][0] * m1[0][1] + m0[3][1] * m1[1][1] + m0[3][2] * m1[2][1] + m0[3][3] * m1[3][1];
		m[3][2] = m0[3][0] * m1[0][2] + m0[3][1] * m1[1][2] + m0[3][2] * m1[2][2] + m0[3][3] * m1[3][2];
		m[3][3] = m0[3][0] * m1[0][3] + m0[3][1] * m1[1][3] + m0[3][2] * m1[2][3] + m0[3][3] * m1[3][3];

		return m;
	}

	friend inline mat4 operator * (const mat4& m0, const float& t)
	{
		return mat4
		(
			m0.e[0] * t, m0.e[1] * t, m0.e[2] * t, m0.e[3] * t,
			m0.e[4] * t, m0.e[5] * t, m0.e[6] * t, m0.e[7] * t,
			m0.e[8] * t, m0.e[9] * t, m0.e[10] * t, m0.e[11] * t,
			m0.e[12] * t, m0.e[13] * t, m0.e[14] * t, m0.e[15] * t
		);
	}

	friend inline mat4 operator / (const mat4& m0, const float& t)
	{
		return mat4
		(
			m0.e[0] / t, m0.e[1] / t, m0.e[2] / t, m0.e[3] / t,
			m0.e[4] / t, m0.e[5] / t, m0.e[6] / t, m0.e[7] / t,
			m0.e[8] / t, m0.e[9] / t, m0.e[10] / t, m0.e[11] / t,
			m0.e[12] / t, m0.e[13] / t, m0.e[14] / t, m0.e[15] / t
		);
	}

	friend inline mat4 operator * (const float& t, const mat4& m0)
	{
		return mat4
		(
			m0.e[0] / t, m0.e[1] / t, m0.e[2] / t, m0.e[3] / t,
			m0.e[4] / t, m0.e[5] / t, m0.e[6] / t, m0.e[7] / t,
			m0.e[8] / t, m0.e[9] / t, m0.e[10] / t, m0.e[11] / t,
			m0.e[12] / t, m0.e[13] / t, m0.e[14] / t, m0.e[15] / t
		);
	}

	inline void init_zero()
	{
		memset(e, 0, MAT4_ELEMENT_COUNT * sizeof(float));
	}

	inline void init_identity()
	{
		init_zero();
		e[0] = 1;
		e[5] = 1;
		e[10] = 1;
		e[15] = 1;
	}

	inline void init_translate(float tx, float ty, float tz)
	{
		init_identity();
		e[3] = tx;
		e[7] = ty;
		e[11] = tz;
	}

	inline void init_rotx(float rx)
	{
		float c = cos(DEG2RAD(rx));
		float s = sin(DEG2RAD(rx));

		init_identity();
		e[5] = c;
		e[9] = s;
		e[6] = -s;
		e[10] = c;
	}

	inline void init_roty(float ry)
	{
		float c = cos(DEG2RAD(ry));
		float s = sin(DEG2RAD(ry));

		init_identity();
		e[0] = c;
		e[8] = -s;
		e[2] = s;
		e[10] = c;
	}

	inline void init_rotz(float rz)
	{
		float c = cos(DEG2RAD(rz));
		float s = sin(DEG2RAD(rz));

		init_identity();
		e[0] = c;
		e[4] = s;
		e[1] = -s;
		e[5] = c;
	}

	inline void initRotateAxisAngle(const vec3& axis, float angle)
	{
		init_identity();

		float fCos = cos(DEG2RAD(-angle));
		float fSin = sin(DEG2RAD(-angle));
		float fOneMinusCos = 1.0f - fCos;
		float fX2 = axis[0] * axis[0];
		float fY2 = axis[1] * axis[1];
		float fZ2 = axis[2] * axis[2];
		float fXYM = axis[0] * axis[1] * fOneMinusCos;
		float fXZM = axis[0] * axis[2] * fOneMinusCos;
		float fYZM = axis[1] * axis[2] * fOneMinusCos;
		float fXSin = axis[0] * fSin;
		float fYSin = axis[1] * fSin;
		float fZSin = axis[2] * fSin;

		e[0] = fX2 * fOneMinusCos + fCos;
		e[1] = fXYM + fZSin;
		e[2] = fXZM - fYSin;
		
		e[4] = fXYM - fZSin;
		e[5] = fY2 * fOneMinusCos + fCos;
		e[6] = fYZM + fXSin;
		
		e[8] = fXZM + fYSin;
		e[9] = fYZM - fXSin;
		e[10] = fZ2 * fOneMinusCos + fCos;
	}

	inline void init_rotate_zxy(float rz, float rx, float ry)
	{
		init_translate_rot_zxy_scale(0, 0, 0, rz, rx, ry, 1);
	}
	//----------------------------------------------------------------------------
	inline void init_rotate_zyx(float rz, float ry, float rx)
	{
		init_translate_rot_zyx_scale(0, 0, 0, rz, ry, rx, 1);
	}
	//----------------------------------------------------------------------------
	inline void init_scale(float scale)
	{
		init_scale(scale, scale, scale);
	}
	//----------------------------------------------------------------------------
	inline void init_scale(float x, float y, float z)
	{
		init_identity();

		e[0] = x;
		e[5] = y;
		e[10] = z;
	}
	//----------------------------------------------------------------------------
	inline void init_translate_rot_zxy_scale(
		float tx, float ty, float tz,
		float rz, float rx, float ry,
		float scale)
	{
		init_identity();

		fromEulerAnglesZXY(rz, rx, ry);
		e[0] *= scale;
		e[4] *= scale;
		e[8] *= scale;

		e[1] *= scale;
		e[5] *= scale;
		e[9] *= scale;

		e[2] *= scale;
		e[6] *= scale;
		e[10] *= scale;

		e[3] = tx;
		e[7] = ty;
		e[11] = tz;
	}
	//----------------------------------------------------------------------------
	inline void init_translate_rot_zyx_scale(float tx, float ty, float tz,
		float rz, float ry, float rx,
		float scale)
	{
		init_identity();

		fromEulerAnglesZYX(rz, ry, rx);

		e[0] *= scale;
		e[4] *= scale;
		e[8] *= scale;
		
		e[1] *= scale;
		e[5] *= scale;
		e[9] *= scale;
		
		e[2] *= scale;
		e[6] *= scale;
		e[10] *= scale;
		
		e[3] = tx;
		e[7] = ty;
		e[11] = tz;
	}

	inline void init_translate_rot_zyx_scale(float tx, float ty, float tz,
		float rz, float ry, float rx,
		float sx, float sy, float sz)
	{
		init_identity();

		fromEulerAnglesZYX(rz, ry, rx);
		e[0] *= sx;
		e[4] *= sx;
		e[8] *= sx;

		e[1] *= sy;
		e[5] *= sy;
		e[9] *= sy;

		e[2] *= sz;
		e[6] *= sz;
		e[10] *= sz;

		e[3] = tx;
		e[7] = ty;
		e[11] = tz;
	}

	//----------------------------------------------------------------------------
	inline void init_translate_scale(float tx, float ty, float tz, float scale)
	{
		init_scale(scale);

		e[3] = tx;
		e[7] = ty;
		e[11] = tz;
	}

	//----------------------------------------------------------------------------
	inline mat4& fromEulerAnglesYZX(float fYAngle, float fPAngle, float fRAngle)
	{
		float fCos, fSin;

		fCos = cos(DEG2RAD(fYAngle));
		fSin = sin(DEG2RAD(fYAngle));
		mat4 kYMat(fCos, 0.0, fSin, 0.0,
			0.0, 1.0, 0.0, 0.0,
			-fSin, 0.0, fCos, 0.0,
			0.0, 0.0, 0.0, 1.0);

		fCos = cos(DEG2RAD(fPAngle));
		fSin = sin(DEG2RAD(fPAngle));
		mat4 kZMat(fCos, -fSin, 0.0, 0.0,
			fSin, fCos, 0.0, 0.0,
			0.0, 0.0, 1.0, 0.0,
			0.0, 0.0, 0.0, 1.0);

		fCos = cos(DEG2RAD(fRAngle));
		fSin = sin(DEG2RAD(fRAngle));
		mat4 kXMat(1.0, 0.0, 0.0, 0.0,
			0.0, fCos, -fSin, 0.0,
			0.0, fSin, fCos, 0.0,
			0.0, 0.0, 0.0, 1.0);

		*this = kYMat * (kZMat * kXMat);
		return *this;
	}
	//----------------------------------------------------------------------------
	inline mat4& fromEulerAnglesZXY(float fYAngle, float fPAngle, float fRAngle)
	{
		float fCos, fSin;

		fCos = cos(DEG2RAD(fYAngle));
		fSin = sin(DEG2RAD(fYAngle));
		mat4 kZMat(fCos, -fSin, 0.0, 0.0,
			fSin, fCos, 0.0, 0.0,
			0.0, 0.0, 1.0, 0.0,
			0.0, 0.0, 0.0, 1.0);

		fCos = cos(DEG2RAD(fPAngle));
		fSin = sin(DEG2RAD(fPAngle));
		mat4 kXMat(1.0, 0.0, 0.0, 0.0,
			0.0, fCos, -fSin, 0.0,
			0.0, fSin, fCos, 0.0,
			0.0, 0.0, 0.0, 1.0);

		fCos = cos(DEG2RAD(fRAngle));
		fSin = sin(DEG2RAD(fRAngle));
		mat4 kYMat(fCos, 0.0, fSin, 0.0,
			0.0, 1.0, 0.0, 0.0,
			-fSin, 0.0, fCos, 0.0,
			0.0, 0.0, 0.0, 1.0);

		*this = kZMat * (kXMat * kYMat);
		return *this;
	}

	//----------------------------------------------------------------------------
	inline mat4& fromEulerAnglesZYX(float fYAngle, float fPAngle, float fRAngle)
	{
		float fCos, fSin;

		fCos = cos(DEG2RAD(fYAngle));
		fSin = sin(DEG2RAD(fYAngle));
		mat4 kZMat(fCos, -fSin, 0.0, 0.0,
			fSin, fCos, 0.0, 0.0,
			0.0, 0.0, 1.0, 0.0,
			0.0, 0.0, 0.0, 1.0);

		fCos = cos(DEG2RAD(fPAngle));
		fSin = sin(DEG2RAD(fPAngle));
		mat4 kYMat(fCos, 0.0, fSin, 0.0,
			0.0, 1.0, 0.0, 0.0,
			-fSin, 0.0, fCos, 0.0,
			0.0, 0.0, 0.0, 1.0);

		fCos = cos(DEG2RAD(fRAngle));
		fSin = sin(DEG2RAD(fRAngle));
		mat4 kXMat(1.0, 0.0, 0.0, 0.0,
			0.0, fCos, -fSin, 0.0,
			0.0, fSin, fCos, 0.0,
			0.0, 0.0, 0.0, 1.0);

		*this = kZMat * (kYMat * kXMat);
		return *this;
	}

	friend inline vec3 operator * (const mat4& m, const vec3& v)
	{
		vec3 result;

		result[0] = m[0][0] * v[0] + m[0][1] * v[1] + m[0][2] * v[2] + m[0][3];
		result[1] = m[1][0] * v[0] + m[1][1] * v[1] + m[1][2] * v[2] + m[1][3];
		result[2] = m[2][0] * v[0] + m[2][1] * v[1] + m[2][2] * v[2] + m[2][3];

		return result;
	}

	inline vec3 mul_position(const vec3& v) const
	{
		const mat4& m = *this;

		vec3 result;
		result[0] = m[0][0] * v[0] + m[0][1] * v[1] + m[0][2] * v[2] + m[0][3];
		result[1] = m[1][0] * v[0] + m[1][1] * v[1] + m[1][2] * v[2] + m[1][3];
		result[2] = m[2][0] * v[0] + m[2][1] * v[1] + m[2][2] * v[2] + m[2][3];
		return result;
	}

	inline vec3 mul_direction(const vec3& v) const
	{
		const mat4& m = *this;
		
		vec3 result;
		result[0] = m[0][0] * v[0] + m[0][1] * v[1] + m[0][2] * v[2];
		result[1] = m[1][0] * v[0] + m[1][1] * v[1] + m[1][2] * v[2];
		result[2] = m[2][0] * v[0] + m[2][1] * v[1] + m[2][2] * v[2];
		return result;
	}

	inline float determinant() const
	{
		const mat4& m = *this;

		float fA0 = m.e[0] * m.e[5] - m.e[1] * m.e[4];
		float fA1 = m.e[0] * m.e[6] - m.e[2] * m.e[4];
		float fA2 = m.e[0] * m.e[7] - m.e[3] * m.e[4];
		float fA3 = m.e[1] * m.e[6] - m.e[2] * m.e[5];
		float fA4 = m.e[1] * m.e[7] - m.e[3] * m.e[5];
		float fA5 = m.e[2] * m.e[7] - m.e[3] * m.e[6];
		float fB0 = m.e[8] * m.e[13] - m.e[9] * m.e[12];
		float fB1 = m.e[8] * m.e[14] - m.e[10] * m.e[12];
		float fB2 = m.e[8] * m.e[15] - m.e[11] * m.e[12];
		float fB3 = m.e[9] * m.e[14] - m.e[10] * m.e[13];
		float fB4 = m.e[9] * m.e[15] - m.e[11] * m.e[13];
		float fB5 = m.e[10] * m.e[15] - m.e[11] * m.e[14];

		return fA0 * fB5 - fA1 * fB4 + fA2 * fB3 + fA3 * fB2 - fA4 * fB1 + fA5 * fB0;
	}

	inline mat4 inverse() const
	{
		const mat4& m = *this;
		mat4 inv;

		float fA0 = m.e[0] * m.e[5] - m.e[1] * m.e[4];
		float fA1 = m.e[0] * m.e[6] - m.e[2] * m.e[4];
		float fA2 = m.e[0] * m.e[7] - m.e[3] * m.e[4];
		float fA3 = m.e[1] * m.e[6] - m.e[2] * m.e[5];
		float fA4 = m.e[1] * m.e[7] - m.e[3] * m.e[5];
		float fA5 = m.e[2] * m.e[7] - m.e[3] * m.e[6];
		float fB0 = m.e[8] * m.e[13] - m.e[9] * m.e[12];
		float fB1 = m.e[8] * m.e[14] - m.e[10] * m.e[12];
		float fB2 = m.e[8] * m.e[15] - m.e[11] * m.e[12];
		float fB3 = m.e[9] * m.e[14] - m.e[10] * m.e[13];
		float fB4 = m.e[9] * m.e[15] - m.e[11] * m.e[13];
		float fB5 = m.e[10] * m.e[15] - m.e[11] * m.e[14];

		float det = fA0 * fB5 - fA1 * fB4 + fA2 * fB3 + fA3 * fB2 - fA4 * fB1 + fA5 * fB0;
		if (abs(det) <= 0.0001)
		{
			inv.init_zero();
			return inv;
		}

		inv[0][0] = +m.e[5]  * fB5 - m.e[6]  * fB4 + m.e[7] * fB3;
		inv[1][0] = -m.e[4]  * fB5 + m.e[6]  * fB2 - m.e[7] * fB1;
		inv[2][0] = +m.e[4]  * fB4 - m.e[5]  * fB2 + m.e[7] * fB0;
		inv[3][0] = -m.e[4]  * fB3 + m.e[5]  * fB1 - m.e[6] * fB0;
		inv[0][1] = -m.e[1]  * fB5 + m.e[2]  * fB4 - m.e[3] * fB3;
		inv[1][1] = +m.e[0]  * fB5 - m.e[2]  * fB2 + m.e[3] * fB1;
		inv[2][1] = -m.e[0]  * fB4 + m.e[1]  * fB2 - m.e[3] * fB0;
		inv[3][1] = +m.e[0]  * fB3 - m.e[1]  * fB1 + m.e[2] * fB0;
		inv[0][2] = +m.e[13] * fA5 - m.e[14] * fA4 + m.e[15] * fA3;
		inv[1][2] = -m.e[12] * fA5 + m.e[14] * fA2 - m.e[15] * fA1;
		inv[2][2] = +m.e[12] * fA4 - m.e[13] * fA2 + m.e[15] * fA0;
		inv[3][2] = -m.e[12] * fA3 + m.e[13] * fA1 - m.e[14] * fA0;
		inv[0][3] = -m.e[9]  * fA5 + m.e[10] * fA4 - m.e[11] * fA3;
		inv[1][3] = +m.e[8]  * fA5 - m.e[10] * fA2 + m.e[11] * fA1;
		inv[2][3] = -m.e[8]  * fA4 + m.e[9]  * fA2 - m.e[11] * fA0;
		inv[3][3] = +m.e[8]  * fA3 - m.e[9]  * fA1 + m.e[10] * fA0;

		float invdet = ((float)1.0) / det;
		for (int iRow = 0; iRow < 4; iRow++)
		{
			for (int iCol = 0; iCol < 4; iCol++)
			{
				inv[iRow][iCol] *= invdet;
			}
		}

		return inv;
	}

	float e[MAT4_ELEMENT_COUNT];
};

#endif