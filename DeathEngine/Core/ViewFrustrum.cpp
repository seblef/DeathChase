
#include "ViewFrustrum.h"

namespace Core
{

ViewFrustrum::ViewFrustrum(const Matrix4 &mat)
{
	Create(mat);
}

void ViewFrustrum::Transform(const Matrix4 &mat)
{
	int i;
	for(i=0;i<6;i++)
		m_Planes[i].Transform(mat);

	m_vPosition*=mat;
}

Vector3 ViewFrustrum::GetFarLeftUp() const
{
	Vector3 p;
	m_Planes[VF_FAR_PLANE].IntersectWithPlanes(
		m_Planes[VF_TOP_PLANE],
		m_Planes[VF_LEFT_PLANE],p);

	return p;
}

Vector3 ViewFrustrum::GetFarLeftDown() const
{
	Vector3 p;
	m_Planes[VF_FAR_PLANE].IntersectWithPlanes(
		m_Planes[VF_BOTTOM_PLANE],
		m_Planes[VF_LEFT_PLANE],p);

	return p;
}

Vector3 ViewFrustrum::GetFarRightUp() const
{
	Vector3 p;
	m_Planes[VF_FAR_PLANE].IntersectWithPlanes(
		m_Planes[VF_TOP_PLANE],
		m_Planes[VF_RIGHT_PLANE],p);

	return p;
}

Vector3 ViewFrustrum::GetFarRightDown() const
{
	Vector3 p;
	m_Planes[VF_FAR_PLANE].IntersectWithPlanes(
		m_Planes[VF_BOTTOM_PLANE],
		m_Planes[VF_RIGHT_PLANE],p);

	return p;
}

void ViewFrustrum::Create(const Matrix4& mat)
{

#define sw(a,b) (mat.m[a][b])

	float len;
	Vector3 norm;
	norm.x=-(sw(0,3) + sw(0,0));
	norm.y=-(sw(1,3) + sw(1,0));
	norm.z=-(sw(2,3) + sw(2,0));
	len=1.0f / norm.GetLength();
	norm*=len;
	m_Planes[VF_LEFT_PLANE].MakePlane(norm,-(sw(3,3) + sw(3,0)) * len);

	norm.x=-(sw(0,3) - sw(0,0));
	norm.y=-(sw(1,3) - sw(1,0));
	norm.z=-(sw(2,3) - sw(2,0));
	len=1.0f / norm.GetLength();
	norm*=len;
	m_Planes[VF_RIGHT_PLANE].MakePlane(norm,-(sw(3,3) - sw(3,0)) * len);

	norm.x=-(sw(0,3) - sw(0,1));
	norm.y=-(sw(1,3) - sw(1,1));
	norm.z=-(sw(2,3) - sw(2,1));
	len=1.0f / norm.GetLength();
	norm*=len;
	m_Planes[VF_TOP_PLANE].MakePlane(norm,-(sw(3,3) - sw(3,1)) * len);

	norm.x=-(sw(0,3) + sw(0,1));
	norm.y=-(sw(1,3) + sw(1,1));
	norm.z=-(sw(2,3) + sw(2,1));
	len=1.0f / norm.GetLength();
	norm*=len;
	m_Planes[VF_BOTTOM_PLANE].MakePlane(norm,-(sw(3,3) + sw(3,1)) * len);
	 
	norm.x=-sw(0,2);
	norm.y=-sw(1,2);
	norm.z=-sw(2,2);
	len=1.0f / norm.GetLength();
	norm*=len;
	m_Planes[VF_NEAR_PLANE].MakePlane(norm,-sw(3,2) * len);

	norm.x=-(sw(0,3) - sw(0,2));
	norm.y=-(sw(1,3) - sw(1,2));
	norm.z=-(sw(2,3) - sw(2,2));
	len=1.0f / norm.GetLength();
	norm*=len;
	m_Planes[VF_FAR_PLANE].MakePlane(norm,-(sw(3,3) - sw(3,2)) * len);

}

bool ViewFrustrum::IsBoxInside(const BBox &box) const
{
	int i;
	for(i=0;i<6;i++)
		if(m_Planes[i].ClassifyBox(box)==PLANE_FRONT)
			return false;

	return true;
}

bool ViewFrustrum::IsPointInside(const Vector3 &v) const
{
	int i;
	for(i=0;i<6;i++)
		if(m_Planes[i].ClassifyPoint(v)==PLANE_FRONT)
			return false;

	return true;
}

bool ViewFrustrum::IsSphereInside(const BSphere& s) const
{
	for(int i=0;i<6;i++)
		if(m_Planes[i].ClassifySphere(s)==PLANE_FRONT)
			return false;

	return true;
}

bool ViewFrustrum::IsLineInside(const Vector3& start, const Vector3& end) const
{
	int i;
	for(i=0;i<6;i++)
		if(m_Planes[i].ClassifyPoint(start)==PLANE_FRONT &&
		   m_Planes[i].ClassifyPoint(end)==PLANE_FRONT)
		   return false;

	return true;
}

}