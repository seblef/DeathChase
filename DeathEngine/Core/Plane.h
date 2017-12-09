
#ifndef _PLANE_H_

#define _PLANE_H_

#include "BBox.h"
#include "BSphere.h"

#define PLANE_FRONT				0
#define PLANE_ON				1
#define PLANE_BACK				2
#define PLANE_CLIP				3

#define PLANE_X					1
#define PLANE_Y					2
#define PLANE_Z					3

namespace Core
{

inline bool PointIsInTriangle(const Vector3& v,
					   const Vector3& v1,
					   const Vector3& v2,
					   const Vector3& v3)
{
	const Vector3 d1(v1-v);
	const Vector3 d2(v2-v);
	const Vector3 d3(v3-v);

	const float a=d1*d2;
	const float b=d1*d3;
	const float c=d2*d3;

	if(a > 0.0f && b > 0.0f && c > 0.0f)
		return false;
	else
		return true;
}

inline void ClosestPointOnLine(const Vector3& v1,
							   const Vector3& v2,
							   const Vector3& p,
							   Vector3& ret)
{
	const Vector3 c(p-v1);
	Vector3 v(v2-v1);
	const float d=v.GetLength();

	v.Normalize();

	const float t=v*c;

	if(t < 0.0f)
		ret=v1;
	else if(t > d)
		ret=v2;
	else
		ret=v1 + v*t;
}

inline void ClosestPointOnTriangle(const Vector3& v1,
							  const Vector3& v2,
							  const Vector3& v3,
							  const Vector3& p,
							  Vector3& ret)
{
	Vector3 ab,bc,ca;
	ClosestPointOnLine(v1,v2,p,ab);
	ClosestPointOnLine(v2,v3,p,bc);
	ClosestPointOnLine(v3,v1,p,ca);

	const float lab=ab.x * ab.x + ab.y * ab.y + ab.z * ab.z;
	const float lbc=bc.x * bc.x + bc.y * bc.y + bc.z * bc.z;
	const float lca=ca.x * ca.x + ca.y * ca.y + ca.z * ca.z;

	if(lab < lbc)
	{
		if(lca < lab) ret=ca; else ret=ab;
	}
	else
	{
		if(lca < lbc) ret=ca; else ret=bc;
	}

}

inline float IntersectSphere(const Vector3& vOrigin,
							 const Vector3& vDir,
							 const Vector3& vCenter,
							 const float fRadius)
{
	Vector3 q(vCenter - vOrigin);
	const float c=q.x * q.x + q.y * q.y + q.z * q.z;
	const float v=q * vDir;
	const float d=fRadius * fRadius - (c - v*v);

	if(d < 0.0f)
		return -1.0f;
	else
		return v-sqrtf(d);
}

inline float IntersectEllipse(const Vector3& vPos,
							  const Vector3& vSize,
							  const Vector3& vOrigin,
							  const Vector3& vDir)
{
	Vector3 vRadius(vSize.x * vDir.x,vSize.y * vDir.y,vSize.z * vDir.z);
	return IntersectSphere(vOrigin,vDir,vPos,vRadius.GetLength());
}

inline float IntersectPlane(const Vector3& vpOrigin,	//	Origine du plan
							const Vector3& vpNormal,	//	Normale du plan (normalisée)
							const Vector3& vrOrigin,	//	Origine du rayon
							const Vector3& vrDir		//	Direction du rayion (normalisée)
							)
{
	const float d=-(vpNormal * vpOrigin);
	const float numer=vpNormal * vrOrigin + d;
	const float denom=vpNormal * vrDir;

	return -(numer / denom);
}

class Plane
{
	public:

		Plane() {};
		Plane(const Plane& _Plane) : m_vNormal(_Plane.m_vNormal),m_fDist(_Plane.m_fDist) {};
		Plane(const Vector3& vNorm, const float fDist) : m_vNormal(vNorm),m_fDist(fDist) {};
		Plane(const Vector3& vPoint, const Vector3& vNorm) : m_vNormal(vNorm)
		{
			m_vNormal=vNorm;
			RecalculateDist(vPoint);
		}
		Plane(const Vector3& vp1, const Vector3& vp2, const Vector3& vp3)
		{
			MakePlane(vp1,vp2,vp3);
		}
		~Plane() {};

		bool operator==(const Plane& _Plane) const { return (m_fDist==_Plane.m_fDist && m_vNormal==_Plane.m_vNormal);};
		bool operator!=(const Plane& _Plane) const { return !(m_fDist==_Plane.m_fDist && m_vNormal==_Plane.m_vNormal);};

		void MakePlane(const Vector3& v, const Vector3& vNorm)
		{
			m_vNormal=vNorm;
			m_vNormal.Normalize();
			RecalculateDist(v);
		}

		void MakePlane(const Vector3& vNorm, const float fDist) { m_vNormal=vNorm; m_fDist=fDist; };

		void MakePlane(const Vector3& v1, const Vector3& v2, const Vector3& v3)
		{
			m_vNormal=(v2-v1)^(v3-v1);
			m_vNormal.Normalize();
			RecalculateDist(v1);
		}

		bool IntersectLine(const Vector3& o, const Vector3& dir,
						   Vector3& vIntersect) const
		{
			const float t2=m_vNormal*dir;

			if(t2==0.0f)
				return false;

			const float t=-(m_vNormal*o + m_fDist) / t2;
			vIntersect=o + dir*t;
			return true;
		}

		float IntersectLineT(const Vector3& o, const Vector3& dir) const
		{
			const float t2=m_vNormal*dir;

			if(t2==0.0f)
				return 0.0f;

			return -(m_vNormal * o + m_fDist) / t2;
		}

		bool IntersectEdge(const Vector3& v1, const Vector3& v2,
						   Vector3& vIntersect) const
		{
			const float t2=m_vNormal * (v2-v1);

			if(t2==0.0f)
				return false;

			const float t=-(m_vNormal*v1 + m_fDist) / t2;

			if(t > 1.0f || t < 0.0f)
				return false;

			vIntersect=v1 + (v2-v1)*t;
			return true;
		}

		int ClassifyPoint(const Vector3& v) const
		{
			const float d=m_vNormal*v + m_fDist;
	
			if(d < -EPSILON)
				return PLANE_BACK;
			else if(d > EPSILON)
				return PLANE_FRONT;
			else
				return PLANE_ON;
		}
		int ClassifySphere(const BSphere& sphere) const
		{
			const float d=m_vNormal*sphere.GetCenter() + m_fDist;
			if(d > sphere.GetRadius()) return PLANE_FRONT;
			else if(d < -sphere.GetRadius()) return PLANE_BACK;
			else return PLANE_ON;
		}
			
		int ClassifyBox(const BBox& box) const
		{
			Vector3 vnear(box.GetMax());
			Vector3 vfar(box.GetMin());

			if(m_vNormal.x > 0.0f)
			{
				vnear.x=box.GetMin().x;
				vfar.x=box.GetMax().x;
			}

			if(m_vNormal.y > 0.0f)
			{
				vnear.y=box.GetMin().y;
				vfar.y=box.GetMax().y;
			}

			if(m_vNormal.z > 0.0f)
			{
				vnear.z=box.GetMin().z;
				vfar.z=box.GetMax().z;
			}

			if((m_vNormal * vnear) + m_fDist > 0.0f)
				return PLANE_FRONT;
			else if((m_vNormal * vfar) + m_fDist > 0.0f)
				return PLANE_CLIP;
			else
				return PLANE_BACK;
		}

		void RecalculateDist(const Vector3& v)
		{
			m_fDist=-(v*m_vNormal);
		}

		Vector3 GetPoint()
		{
			return m_vNormal*(-m_fDist);
		}

		bool ExistPlaneIntersection(const Plane& _Plane) const
		{
			Vector3 vCross(_Plane.m_vNormal ^ m_vNormal);
			return vCross.GetLength() > 1e-08f;
		}
		bool IntersectPlane(const Plane& _Plane, Vector3& o, Vector3& dir) const
		{
			const float fn00=m_vNormal.GetLength();
			const float fn01=m_vNormal * _Plane.m_vNormal;
			const float fn11=_Plane.m_vNormal.GetLength();
			float det=fn00*fn11 - fn01*fn01;

			if(fabs(det) < 1e-08f)
				return false;

			det=1.0f / det;
			const float fc0=(fn11*(-m_fDist) + fn01 * _Plane.m_fDist) * det;
			const float fc1=(fn00*(-_Plane.m_fDist) + fn01 * m_fDist) * det;

			dir=m_vNormal ^ _Plane.m_vNormal;
			o=m_vNormal * fc0 + _Plane.m_vNormal*fc1;

			return true;
		}
		bool IntersectWithPlanes(const Plane& p1, const Plane& p2, Vector3& vIntersect) const
		{
			Vector3 line_pt,line_dir;
			if(IntersectPlane(p1,line_pt,line_dir))
				return p2.IntersectLine(line_pt, line_dir,vIntersect);

			return false;
		}

		bool IsFront(const Vector3& vLookDir) const
		{
			return (m_vNormal * vLookDir) <= 0.0f;
		}

		float GetDistanceFromPoint(const Vector3& v) const
		{
				return m_vNormal*v + m_fDist;
		}

		const Vector3& GetNormal() const { return m_vNormal; };
		float GetDist() const { return m_fDist; };

		void Transform(const Matrix4& mat)
		{
			Vector3 m(GetPoint() * mat);
			Vector3 o(0.0f,0.0f,0.0f);

			m_vNormal*=mat;
			o*=mat;

			m_vNormal-=o;
			m_fDist=-(m*m_vNormal);
		}

	protected:
		
		Vector3 m_vNormal;
		float m_fDist;

};

}

#endif