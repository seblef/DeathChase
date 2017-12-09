
#ifndef _VECTOR_H_

#define _VECTOR_H_

#include "Matrix.h"

namespace Core
{

class Vector2i;

class Vector2
{
	public:

		static Vector2 NullVector;
		static Vector2 XAxisVector;
		static Vector2 YAxisVector;

		Vector2() {};
		Vector2(const float _x,
				  const float _y) {x=_x; y=_y; };
		Vector2(const Vector2& v) {x=v.x; y=v.y; };
		Vector2(const Vector2i& v);

		~Vector2() {};

		const Vector2& operator=(const Vector2& v) {x=v.x; y=v.y; return *this; };
		const Vector2& operator=(const Vector2i& v);

		bool operator==(const Vector2& v) const { return ((v.x==x) && (v.y==y)); };
		bool operator!=(const Vector2& v) const { return ((v.x!=x) || (v.y!=y)); };

		Vector2 operator+(const Vector2& v) const { return Vector2(x+v.x, y+v.y); };
		Vector2 operator+(const Vector2i& v) const;

		Vector2 operator-() const { return Vector2(-x,-y); };

		const Vector2& operator+=(const Vector2& v) { x+=v.x; y+=v.y; return *this; };
		const Vector2& operator+=(const Vector2i& v);

		Vector2 operator-(const Vector2& v) const { return Vector2(x-v.x, y-v.y); };
		Vector2 operator-(const Vector2i& v) const;

		const Vector2& operator-=(const Vector2& v) { x-=v.x; y-=v.y; return *this; };
		const Vector2& operator-=(const Vector2i& v);

		float operator*(const Vector2& v) const { return x*v.x + y*v.y; };
		Vector2 operator*(const float f) const { return Vector2(x*f, y*f); };
		const Vector2& operator*=(const float f) { x*=f; y*=f; return *this; };

		Vector2 operator/(const float f) const { const float inv=KINV(f); return Vector2(x*inv, y*inv); };
		const Vector2& operator/=(const float f) { const float inv=KINV(f); x*=inv; y*=inv; return *this; };

		const float GetLength() const { return (float)sqrt(x*x + y*y); };
		const float GetSqrLength() const { return x*x+y*y; };
		void Normalize() {const float inv=FastInvSqrt(x*x+y*y); x*=inv; y*=inv;};

		float x,y;

};

class Vector2i
{
	public:

		static Vector2i NullVector;
		static Vector2i XAxisVector;
		static Vector2i YAxisVector;

		Vector2i() {};
		Vector2i(const int _x,
				  const int _y) {x=_x; y=_y; };
		Vector2i(const Vector2i& v) {x=v.x; y=v.y; };
		Vector2i(const Vector2& v) { x=(int)v.x; y=(int)v.y; };

		~Vector2i() {};

		const Vector2i& operator=(const Vector2i& v) {x=v.x; y=v.y; return *this; };
		const Vector2i& operator=(const Vector2& v) { x=(int)v.x; y=(int)v.y; return *this; };

		bool operator==(const Vector2i& v) const { return ((v.x==x) && (v.y==y)); };
		bool operator!=(const Vector2i& v) const { return ((v.x!=x) || (v.y!=y)); };

		Vector2i operator-() const { return Vector2i(-x,-y); };

		Vector2i operator+(const Vector2i& v) const { return Vector2i(x+v.x, y+v.y); };
		Vector2i operator+(const Vector2& v) const { return Vector2i(x+(int)v.x, y+(int)v.y); };

		const Vector2i& operator+=(const Vector2i& v) { x+=v.x; y+=v.y; return *this; };
		const Vector2i& operator+=(const Vector2& v) { x+=(int)v.x; y+=(int)v.y; return *this; };

		Vector2i operator-(const Vector2i& v) const { return Vector2i(x-v.x, y-v.y); };
		Vector2i operator-(const Vector2& v) const { return Vector2i(x-(int)v.x, y-(int)v.y); };

		const Vector2i& operator-=(const Vector2i& v) { x-=v.x; y-=v.y; return *this; };
		const Vector2i& operator-=(const Vector2& v) { x-=(int)v.x; y-=(int)v.y; return *this; };

		int operator*(const Vector2i& v) const { return x*v.x + y*v.y; };

		Vector2i operator*(const int f) const { return Vector2i(x*f, y*f); };
		Vector2i operator*(const float f) const { return Vector2i((int)((float)x * f),(int)((float)y * f)); };

		const Vector2i& operator*=(const int f) { x*=f; y*=f; return *this; };
		const Vector2i& operator*=(const float f) { x=(int)((float)x * f); y=(int)((float)y * f); return *this; };

		Vector2i operator/(const int f) const { return Vector2i(x/f,y/f); };
		const Vector2i& operator/=(const int f) { x/=f; y/=f; return *this; };

		const float GetLength() const { return (float)sqrt((float)(x*x + y*y)); };
		const int GetSqrLength() const { return x*x+y*y; };
		void Normalize() {const float inv=FastInvSqrt((float)x*x+y*y); x=(int)((float)x*inv); y=(int)((float)y * inv);};

		int x,y;

};

inline Vector2::Vector2(const Vector2i& v) { x=(float)v.x; y=(float)v.y; };
inline const Vector2& Vector2::operator=(const Vector2i& v)  { x=(float)v.x; y=(float)v.y; return *this; };
inline Vector2 Vector2::operator+(const Vector2i& v) const { return Vector2(x+(float)v.x, y+(float)v.y); };
inline const Vector2& Vector2::operator+=(const Vector2i& v) { x+=(float)v.y; y+=(float)v.y; return *this; };
inline Vector2 Vector2::operator-(const Vector2i& v) const { return Vector2(x-(float)v.x, y-(float)v.y); };
inline const Vector2& Vector2::operator-=(const Vector2i& v)  { x-=(float)v.x; y-=(float)v.y; return *this; };

class Vector3
{
	public:

		static Vector3 NullVector;
		static Vector3 XAxisVector;
		static Vector3 YAxisVector;
		static Vector3 ZAxisVector;

		Vector3() {};
		Vector3(const float _x,
				  const float _y,
				  const float _z) {x=_x; y=_y; z=_z; };
		Vector3(const Vector3& v) {x=v.x; y=v.y; z=v.z; };
		Vector3(const float *pv) { x=*pv; y=*(pv+1); z=*(pv+2); };

		~Vector3() {};

		const Vector3& operator=(const Vector3& v) {x=v.x; y=v.y; z=v.z; return *this; };
		bool operator==(const Vector3& v) const { return ((v.x==x) && (v.y==y) && (v.z==z)); };

		Vector3 operator-() const { return Vector3(-x,-y,-z); };

		Vector3 operator+(const Vector3& v) const { return Vector3(x+v.x, y+v.y, z+v.z); };
		Vector3 operator+(const float *pv) const { return Vector3(x+(*pv),y+(*(pv+1)),z+(*(pv+2))); };

		const Vector3& operator+=(const Vector3& v) { x+=v.x; y+=v.y; z+=v.z; return *this; };
		const Vector3& operator+=(const float *pv) { x+=*pv; y+=*(pv+1); z+=*(pv+2); return *this; };

		Vector3 operator-(const Vector3& v) const { return Vector3(x-v.x, y-v.y, z-v.z); };
		Vector3 operator-(const float *pv) const { return Vector3(x-(*pv),y-(*(pv+1)),z-(*(pv+2))); };

		const Vector3& operator-=(const Vector3& v) { x-=v.x; y-=v.y; z-=v.z; return *this; };
		const Vector3& operator-=(const float *pv) { x-=*pv; y-=*(pv+1); z-=*(pv+2); return *this; };

		float operator*(const Vector3& v) const { return x*v.x + y*v.y + z*v.z; };
		float operator*(const float *pv) const { return x*(*pv) + y*(*(pv+1)) + z*(*(pv+2)); };

		Vector3 operator*(const float f) const { return Vector3(x*f, y*f, z*f); };
		const Vector3& operator*=(const float f) { x*=f; y*=f; z*=f; return *this; };
		Vector3 operator*(const Matrix4& m) const { return Vector3(x*m.m[0][0] + y*m.m[1][0] + z*m.m[2][0] + m.m[3][0],
																	     x*m.m[0][1] + y*m.m[1][1] + z*m.m[2][1] + m.m[3][1],
																	     x*m.m[0][2] + y*m.m[1][2] + z*m.m[2][2] + m.m[3][2]); };
		const Vector3& operator*=(const Matrix4& m) { const float tx=x;
														  const float ty=y;
														  const float tz=z;

														  x=tx*m.m[0][0] + ty*m.m[1][0] + tz*m.m[2][0] + m.m[3][0];
														  y=tx*m.m[0][1] + ty*m.m[1][1] + tz*m.m[2][1] + m.m[3][1];
														  z=tx*m.m[0][2] + ty*m.m[1][2] + tz*m.m[2][2] + m.m[3][2];

														  return *this; };

		Vector3 operator/(const float f) const { const float inv=KINV(f); return Vector3(x*inv, y*inv, z*inv); };
		const Vector3& operator/=(const float f) { const float inv=KINV(f); x*=inv; y*=inv; z*=inv; return *this; };

		Vector3 operator^(const Vector3& v) const { return Vector3(y*v.z - z*v.y,
																	     z*v.x - x*v.z,
																	     x*v.y - y*v.x); };

		const float GetLength() const { return (float)sqrtf(x*x + y*y + z*z); };
		const float GetSquareLength() const { return x*x + y*y + z*z; };
		void Normalize()
		{
			const float fNorm=x*x+y*y+z*z;
			if((fabsf(fNorm-1.0f) > SQREPSILON) && (fNorm > SQREPSILON))
			{
				const float fInvNorm=FastInvSqrt(fNorm);
				x*=fInvNorm; y*=fInvNorm; z*=fInvNorm;
			}
		}
		Vector3 Negative() const { return Vector3(-x,-y,-z); };

		float GetDistanceFrom(const Vector3& v) const { return Vector3(x-v.x,y-v.y,z-v.z).GetLength(); };
		float GetDistanceFromSqr(const Vector3& v) const { return Vector3(x-v.x,y-v.y,z-v.z).GetSquareLength(); };

		bool IsBetween(const Vector3& v1, const Vector3& v2) const
		{
			const float f=(v2-v1).GetSquareLength();
			return (GetDistanceFromSqr(v1) < f && GetDistanceFromSqr(v2) < f);
		};

		float x,y,z;

};

}

#endif