
#ifndef _QUATERNION_H_

#define _QUATERNION_H_

#include "Vector.h"

namespace Core
{

class Quaternion
{
	public:

		float x,y,z,w;

		Quaternion() : x(0.0f),y(0.0f),z(0.0f),w(1.0f) {};
		Quaternion(float _x, float _y, float _z, float _w) : x(_x),y(_y),z(_z),w(_w) {};
		Quaternion(const Quaternion& quat) : x(quat.x),y(quat.y),z(quat.z),w(quat.w) {};
		Quaternion(const Matrix4& mat) { BuildMatrix(mat); };
		Quaternion(const Vector3& vAxis, const float fAngle) { BuildAxisAngle(vAxis,fAngle); };
		~Quaternion() {};

		void BuildMatrix(const Matrix4& mat)
		{
			const float fTrace=mat.m[0][0] + mat.m[1][1] + mat.m[2][2];
			float fRoot;
			if(fTrace > 0.0f)
			{
				fRoot=sqrtf(fTrace + 1.0f);
				w=0.5f*fRoot;
				fRoot=0.5f / fRoot;
				x=(mat.m[2][1] - mat.m[1][2])*fRoot;
				y=(mat.m[0][2] - mat.m[2][0])*fRoot;
				z=(mat.m[1][0] - mat.m[0][1])*fRoot;
			}
			else
			{
				const int iNext[3]={1,2,0};
				int i=0;
				if(mat.m[1][1] > mat.m[0][0])
					i=1;

				if(mat.m[2][2] > mat.m[i][i])
					i=2;

				const int j=iNext[i]; 
				const int k=iNext[j];
				fRoot=sqrtf(mat.m[i][i] - mat.m[j][j] - mat.m[k][k] + 1.0f);
				float *pQuat[3]={&x,&y,&z};
				*pQuat[i]=0.5f*fRoot;

				fRoot=0.5f/fRoot;
				w=(mat.m[k][j] - mat.m[j][k])*fRoot;
				*(pQuat[j])=(mat.m[j][i] + mat.m[i][j])*fRoot;
				*(pQuat[k])=(mat.m[k][i] + mat.m[i][k])*fRoot;
			}
			Normalize();
		};
		void BuildAxisAngle(const Vector3& vAxis, const float fAngle)
		{
			const float fHalfAngle=fAngle*0.5f;
			const float fSin=sinf(fHalfAngle);
			x=fSin*vAxis.x;
			y=fSin*vAxis.y;
			z=fSin*vAxis.z;
			w=cosf(fHalfAngle);

			Normalize();
		};
		void BuildEuler(const Vector3& vRot) { BuildEuler(vRot.x,vRot.y,vRot.z); };
		void BuildEuler(const float ex, const float ey, const float ez)
		{
			float fAngle,sr,sp,sy,cr,cp,cy;
			fAngle=ez*0.5f; sy=sinf(fAngle); cy=cosf(fAngle);
			fAngle=ey*0.5f; sp=sinf(fAngle); cp=cosf(fAngle);
			fAngle=ex*0.5f; sr=sinf(fAngle); cr=cosf(fAngle);

			const float cpcy=cp*cy;
			const float spcy=sp*cy;
			const float cpsy=cp*sy;
			const float spsy=sp*sy;

			x=sr * cpcy - cr * spsy;
			y=cr * spcy + sr * cpsy;
			z=cr * cpsy - sr * spcy;
			w=cr * cpcy + sr * spsy;
			Normalize();
		};
		void BuildRotationArc(const Vector3& vFrom, const Vector3& vTo) {};
		void GetMatrix(Matrix4& mat) const
		{
			const float fTX=2.0f*x, fTY=2.0f*y, fTZ=2.0f*z;
			const float fTWX=fTX*w, fTWY=fTY*w, fTWZ=fTZ*w;
			const float fTXX=fTX*x, fTXY=fTY*x, fTXZ=fTZ*x;
			const float fTYY=fTY*y, fTYZ=fTZ*y;
			const float fTZZ=fTZ*z;
			mat.m[0][0]=1.0f - (fTYY+fTZZ); mat.m[0][1]=fTXY - fTWZ;		mat.m[0][2]=fTXZ + fTWY;		mat.m[0][3]=0.0f;
			mat.m[1][0]=fTXY + fTWZ;		mat.m[1][1]=1.0f - (fTXX+fTZZ); mat.m[1][2]=fTYZ - fTWX;		mat.m[1][3]=0.0f;
			mat.m[2][0]=fTXZ - fTWY;		mat.m[2][1]=fTYZ + fTWX;		mat.m[2][2]=1.0f - (fTXX+fTYY); mat.m[2][3]=0.0f;
			mat.m[3][0]=0.0f;				mat.m[3][1]=0.0f;				mat.m[3][2]=0.0f;				mat.m[3][3]=1.0f;
		};
		void GetAxisAngle(Vector3& vAxis, float &fAngle) const
		{
			const float fSqrLen=x*x+y*y+z*z;
			if(fSqrLen > 0.0f)
			{
				fAngle=2.0f * acosf(w);
				const float fInvLen=1.0f / logf(fSqrLen);
				vAxis.x=x*fInvLen; vAxis.y=y*fInvLen; vAxis.z=z*fInvLen;
			}
			else
			{
				fAngle=0.0f;
				vAxis.x=1.0f; vAxis.y=0.0f; vAxis.z=0.0f;
			}
		};
		Quaternion GetInverse() const { return Quaternion(-x,-y,-z,w); };

		void Normalize()
		{
			const float fNorm=x*x + y*y + z*z + w*w;
			if(fNorm < EPSILON) { x=y=z=0.0f; w=1.0f; }
			else if(fabsf(fNorm - 1.0f) > EPSILON)
			{
				const float fScale=FastInvSqrt(fNorm);
				x*=fScale; y*=fScale; z*=fScale; w*=fScale;
			}
		};
		void Reset() { x=y=z=0.0f; w=1.0f; };
		void Inverse() { x=-x; y=-y; z=-z; };

		Quaternion operator+(const Quaternion &quat) const
		{ return Quaternion(x+quat.x,y+quat.y,quat.z,w+quat.w); };
		const Quaternion &operator+=(const Quaternion& quat)
		{ x+=quat.x; y+=quat.y; z+=quat.z; w+=quat.w; return *this; };

		Quaternion operator-(const Quaternion& quat) const
		{ return Quaternion(x-quat.x,y-quat.y,z-quat.z,w-quat.w); };
		const Quaternion &operator-=(const Quaternion& quat)
		{ x-=quat.x; y-=quat.y; z-=quat.z; w-=quat.w; return *this; };

		Quaternion operator*(const Quaternion& quat) const
		{
			Vector3 v1(x,y,z),v2(quat.x,quat.y,quat.z);
			Vector3 res((v1^v2) + v2*w + v1*quat.w);

			return Quaternion(res.x,res.y,res.z,w*quat.w - v1*v2);
		};
		const Quaternion& operator*=(const Quaternion& quat)
		{
			Vector3 v1(x,y,z),v2(quat.x,quat.y,quat.z);
			Vector3 res((v1^v2) + v2*w + v1*quat.w);

			x=res.x; y=res.y; z=res.z;
			w=w*quat.w - v1*v2;
			return *this;
		};

		Quaternion operator*(const float fact) const
		{ return Quaternion(x*fact,y*fact,z*fact,w*fact); };
		const Quaternion& operator*=(const float fact)
		{ x*=fact; y*=fact; z*=fact; w*=fact; return *this; };

		Vector3 operator*(Vector3& v)
		{
			Vector3 v0(x,y,z);
			Vector3 v1(v0 ^ v);
			v1+=v*w;
			Vector3 v2(v1 ^ v0);
			v0*=v*v0;
			v0+=v1*w;

			return Vector3(v0-v2);
		};

		bool operator==(const Quaternion& quat) const
		{	return (x==quat.x) && (y==quat.y) && (z==quat.z) && (w==quat.w); };
		bool operator!=(const Quaternion& quat) const
		{	return (x!=quat.x) || (y!=quat.y) || (z!=quat.z) || (w!=quat.w); };

		const Quaternion& operator=(const Quaternion& quat) { x=quat.x; y=quat.y; z=quat.z; w=quat.w; return *this; };

};

}

#endif
