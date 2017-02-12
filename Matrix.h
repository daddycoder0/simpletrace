#ifndef _MY_MATRIX_H_
#define _MY_MATRIX_H_

#include <math.h>
#include "ThirdParty/rapidxml/rapidxml.hpp"

#define EPSILON 0.0000000000000000001f
#define DEG2RAD 0.0174532925199432957f

typedef float real;

class Vector3
{
public:
	inline Vector3()								{};
	inline Vector3(real x, real y, real z)			{m_x = x; m_y = y; m_z = z;}
	inline Vector3(real s)							{m_x = m_y = m_z = s;}
	inline Vector3(double x, double y, double z)	{m_x = (real)x;m_y = (real)y;m_z = (real)z;}
	
	inline void Set(real x, real y, real z)			{m_x = x; m_y = y; m_z = z;}
	inline void Set(double x, double y, double z)	{m_x = (real)x;m_y = (real)y;m_z = (real)z;}
	inline void Set(real s)							{m_x = m_y = m_z = s;}
	inline Vector3& Normalize()
	{
		const real i(1.0f /(EPSILON+Length()));
		m_x *= i; m_y *= i; m_z *= i;
		return *this;
	}


	inline Vector3& Add( const Vector3 &v1, const Vector3 &v2 )
	{
		m_x = v1.m_x + v2.m_x;
		m_y = v1.m_y + v2.m_y;
		m_z = v1.m_z + v2.m_z;
		return *this;
	}

	inline Vector3& Sub( const Vector3 &v1, const Vector3 &v2 )
	{
		m_x = v1.m_x - v2.m_x;
		m_y = v1.m_y - v2.m_y;
		m_z = v1.m_z - v2.m_z;
		return *this;
	}

	inline Vector3& Mul( const Vector3 &v, real s )
	{
		m_x = v.m_x * s;
		m_y = v.m_y * s;
		m_z = v.m_z * s;
		return *this;
	}
	
	inline Vector3 operator+ (Vector3& v)
	{	
		return Vector3( m_x+v.m_x, m_y+v.m_y, m_z+v.m_z );
	}

	inline Vector3 operator* (Vector3& v)
	{	
		return Vector3( m_x*v.m_x, m_y*v.m_y, m_z*v.m_z );
	}

	inline Vector3 operator* (float s)
	{	
		return Vector3( m_x*s, m_y*s, m_z*s );
	}

	inline Vector3 operator/ ( float s )
	{
		return Vector3( m_x/s, m_y/s, m_z/s );
	}

	inline real Length() const
	{
		return (real)sqrtf( m_x*m_x + m_y*m_y + m_z*m_z );
	}

	inline Vector3 Cross( const Vector3 &v1, const Vector3 &v2 )
	{
		Vector3 temp;
		temp.m_x = v1.m_y * v2.m_z - v1.m_z * v2.m_y;
		temp.m_y = v1.m_z * v2.m_x - v1.m_x * v2.m_z;
		temp.m_z = v1.m_x * v2.m_y - v1.m_y * v2.m_x;
		*this = temp;
		return *this;
	}

	inline Vector3 Lerp( const Vector3& v1, const Vector3 v2, float t )
	{
		Vector3 temp;
		float oneminust = 1.f - t;
		temp.m_x = (t * v1.m_x) + (oneminust * v2.m_x);
		temp.m_y = (t * v1.m_y) + (oneminust * v2.m_y);
		temp.m_z = (t * v1.m_z) + (oneminust * v2.m_z);
		*this = temp;
		return *this;
	}

	bool Parse(rapidxml::xml_node<>* node, char* sp, int spSize);

	real m_x, m_y, m_z;

	friend class Matrix4;
	friend class Vector4;
};


class Vector4
{
public:
	inline Vector4()										{};
	inline Vector4(real x, real y, real z, real w)			{m_x = x; m_y = y; m_z = z; m_w = w;}
	inline Vector4(real s)									{m_x = m_y = m_z = m_w = s;}
	inline Vector4(double x, double y, double z, double w)	{m_x = (real)x;m_y = (real)y;m_z = (real)z;m_w = (real)w;}
	inline Vector4(Vector3& v, real w)						{m_x = v.m_x; m_y = v.m_y; m_z = v.m_z; m_w = w;}
	inline void Set(real x, real y, real z, real w)			{m_x = x; m_y = y; m_z = z; m_w = w;}
	inline void Set(double x, double y, double z, double w)	{m_x = (real)x;m_y = (real)y;m_z = (real)z;m_w = (real)w;}
	inline void Set(Vector3& v, real w)						{m_x = v.m_x; m_y = v.m_y; m_z = v.m_z; m_w = w;}
	inline real operator() ( int i )						{return *((&m_x) + i);}

	inline Vector4& Normalize()
	{
		const real i(1.0f /(EPSILON+Length()));
		m_x *= i; m_y *= i; m_z *= i; m_w *= i;
		return *this;
	}


	inline Vector4& Add( const Vector4 &v1, const Vector4 &v2 )
	{
		m_x = v1.m_x + v2.m_x;
		m_y = v1.m_y + v2.m_y;
		m_z = v1.m_z + v2.m_z;
		m_w = v1.m_w + v2.m_w;
		return *this;
	}

	inline Vector4& Sub( const Vector4 &v1, const Vector4 &v2 )
	{
		m_x = v1.m_x - v2.m_x;
		m_y = v1.m_y - v2.m_y;
		m_z = v1.m_z - v2.m_z;
		m_w = v1.m_w - v2.m_w;
		return *this;
	}

	inline Vector4& Mul( const Vector4 &v, real s )
	{
		m_x = v.m_x * s;
		m_y = v.m_y * s;
		m_z = v.m_z * s;
		m_w = v.m_w * s;
		return *this;
	}

	inline real Length() const
	{
		return (real)sqrtf( m_x*m_x + m_y*m_y + m_z*m_z + m_w*m_w );
	}

	real m_x, m_y, m_z, m_w;

	friend class Matrix4;
};

class Matrix3
{
private:
	real m[3][3];
};

class Matrix4
{
public:
	Matrix4 (){};
	inline Matrix4( const Vector4 &x, const Vector4 &y, const Vector4 &z, const Vector4 &w )
	{
		m[0][0] = x.m_x; m[0][1] = x.m_y; m[0][2] = x.m_z; m[0][3] = x.m_w;
		m[1][0] = y.m_x; m[1][1] = y.m_y; m[1][2] = y.m_z; m[1][3] = y.m_w;
		m[2][0] = z.m_x; m[2][1] = z.m_y; m[2][2] = z.m_z; m[2][3] = z.m_w;
		m[3][0] = w.m_x; m[3][1] = w.m_y; m[3][2] = w.m_z; m[3][3] = w.m_w;
	}

	inline void Set( const Vector4 &x, const Vector4 &y, const Vector4 &z, const Vector4 &w )
	{
		m[0][0] = x.m_x; m[0][1] = x.m_y; m[0][2] = x.m_z; m[0][3] = x.m_w;
		m[1][0] = y.m_x; m[1][1] = y.m_y; m[1][2] = y.m_z; m[1][3] = y.m_w;
		m[2][0] = z.m_x; m[2][1] = z.m_y; m[2][2] = z.m_z; m[2][3] = z.m_w;
		m[3][0] = w.m_x; m[3][1] = w.m_y; m[3][2] = w.m_z; m[3][3] = w.m_w;
	}

	inline Vector3 Transform( const Vector3 &localSpacePoint ) const
	{
		return Vector3(
			m[0][0]*localSpacePoint.m_x + m[1][0]*localSpacePoint.m_y + m[2][0]*localSpacePoint.m_z + m[3][0],
			m[0][1]*localSpacePoint.m_x + m[1][1]*localSpacePoint.m_y + m[2][1]*localSpacePoint.m_z + m[3][1],
			m[0][2]*localSpacePoint.m_x + m[1][2]*localSpacePoint.m_y + m[2][2]*localSpacePoint.m_z + m[3][2] );
	}

	inline Vector3 InverseTransform( const Vector3 &parentSpacePoint ) const
	{
		real tx = parentSpacePoint.m_x - m[3][0];
		real ty = parentSpacePoint.m_y - m[3][1];
		real tz = parentSpacePoint.m_z - m[3][2];
		return Vector3(
			m[0][0]*tx + m[0][1]*ty + m[0][2]*tz,
			m[1][0]*tx + m[1][1]*ty + m[1][2]*tz,
			m[2][0]*tx + m[2][1]*ty + m[2][2]*tz );
	}
	inline void SetElement(int index, real val){m[index/4][index%4] = val;}

	void GetVectors(Vector3& x, Vector3& y, Vector3& z, Vector3& p)
	{
		x.Set(m[0][0], m[0][1], m[0][2]);
		y.Set(m[1][0], m[1][1], m[1][2]);
		z.Set(m[2][0], m[2][1], m[2][2]);
		p.Set(m[3][0], m[3][1], m[3][2]);
	}

	inline Vector3 Pos()
	{
		return Vector3(m[3][0], m[3][1], m[3][2]);
	}

	void SetPos(real x, real y, real z)
	{
		m[3][0] = x;
		m[3][1] = y;
		m[3][2] = z;
	}

	inline void SetRotateX( real radians )
	{
		real _cos = cos( radians );
		real _sin = sin( radians );
		m[0][0] = 1.0f; m[0][1] = 0.0f; m[0][2] =  0.0f; m[0][3] = 0.0f;
		m[1][0] = 0.0f; m[1][1] = _cos;  m[1][2] = -_sin;  m[1][3] = 0.0f;
		m[2][0] = 0.0f; m[2][1] = _sin;  m[2][2] =  _cos;  m[2][3] = 0.0f;
		m[3][0] = 0.0f; m[3][1] = 0.0f; m[3][2] =  0.0f; m[3][3] = 1.0f;
	}

	inline void SetRotateY( real radians )
	{
		real _cos = cos( radians );
		real _sin = sin( radians );
		m[0][0] = _cos;  m[0][1] = 0.0f; m[0][2] = -_sin;  m[0][3] = 0.0f;
		m[1][0] = 0.0f; m[1][1] = 1.0f; m[1][2] =  0.0f; m[1][3] = 0.0f;
		m[2][0] = _sin;  m[2][1] = 0.0f; m[2][2] =  _cos;  m[2][3] = 0.0f;
		m[3][0] = 0.0f; m[3][1] = 0.0f; m[3][2] =  0.0f; m[3][3] = 1.0f;
	}

	inline void SetRotateZ( real radians )
	{
		real _cos = cos( radians );
		real _sin = sin( radians );
		m[0][0] = _cos;  m[0][1] = -_sin;  m[0][2] = 0.0f; m[0][3] = 0.0f;
		m[1][0] = _sin;  m[1][1] =  _cos;  m[1][2] = 0.0f; m[1][3] = 0.0f;
		m[2][0] = 0.0f; m[2][1] =  0.0f; m[2][2] = 1.0f; m[2][3] = 0.0f;
		m[3][0] = 0.0f; m[3][1] =  0.0f; m[3][2] = 0.0f; m[3][3] = 1.0f;
	}

	void InitRotFromAngles(real x, real y, real z);

	bool Parse(rapidxml::xml_node<>* node, char* sp, int spSize);
	void SetToIdentity();

private:
	real m[4][4];

};

inline Vector3 CrossProduct( const Vector3 &vec1, const Vector3 &vec2 )
{
	Vector3 Vnew;
	return Vnew.Cross( vec1, vec2 );
}

inline Vector3 InterpolateVectors( const Vector3 &vec1, const Vector3& vec2, float t )
{
	Vector3 Vnew;
	return Vnew.Lerp( vec1, vec2, t);
}

inline real DotProduct( const Vector3 &vec1, const Vector3 &vec2 )
{
	return vec1.m_x * vec2.m_x + vec1.m_y * vec2.m_y + vec1.m_z * vec2.m_z;
}

char* strtk(char* tok, char** context);

#ifdef WIN32
	#define snprintf sprintf_s	// these aren't quite the same but for the benefit of working on linux and windows, should be sufficient
#endif

#endif