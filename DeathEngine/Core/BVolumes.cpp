
#include "BBox.h"
#include "BSphere.h"

namespace Core
{

BBox::BBox(const BSphere *pSphere) : m_vMin(pSphere->GetCenter()), m_vMax(pSphere->GetCenter())
{
	const float fRad=pSphere->GetRadius();
	m_vMin.x-=fRad; m_vMin.y-=fRad; m_vMin.z-=fRad;
	m_vMax.x+=fRad; m_vMax.y+=fRad; m_vMax.x+=fRad;
}

void BBox::ToSphere(BSphere *pSphere) const
{
	const Vector3 vCenter((m_vMin + m_vMax) * 0.5f);
	pSphere->SetCenter(vCenter);
	pSphere->SetRadius(((m_vMax - m_vMin) * 0.5f).GetLength());
}

void BBox::FromSphere(const BSphere *pSphere)
{
	const Vector3 vRadius(pSphere->GetRadius(),pSphere->GetRadius(),pSphere->GetRadius());
	m_vMin=m_vMax=pSphere->GetCenter();
	m_vMin-=vRadius;
	m_vMax+=vRadius;
}

bool BBox::IsIn(const BSphere *pSphere, bool bCompletely) const
{
	const Vector3& vCenter=pSphere->GetCenter();
	const float& fRadius=pSphere->GetRadius();

	if(bCompletely)
	{
		if((vCenter.x + fRadius) > m_vMax.x || (vCenter.x - fRadius) < m_vMin.x) return false;
		if((vCenter.y + fRadius) > m_vMax.y || (vCenter.y - fRadius) < m_vMin.y) return false;
		if((vCenter.z + fRadius) > m_vMax.z || (vCenter.z - fRadius) < m_vMin.z) return false;
	}
	else
	{
		if((vCenter.x + fRadius) < m_vMin.x || (vCenter.x - fRadius) > m_vMax.x) return false;
		if((vCenter.y + fRadius) < m_vMin.y || (vCenter.y - fRadius) > m_vMax.y) return false;
		if((vCenter.z + fRadius) < m_vMin.z || (vCenter.z - fRadius) > m_vMax.z) return false;
	}
	
	return true;
}


BSphere::BSphere(const BBox *pBBox) : m_vCenter(pBBox->GetCenter())
{
	Adjust(pBBox->GetMax());
}

void BSphere::ToBBox(BBox *pBBox) const
{
	pBBox->m_vMin=pBBox->m_vMax=m_vCenter;
	pBBox->m_vMin.x-=m_fRadius;
	pBBox->m_vMin.y-=m_fRadius;
	pBBox->m_vMin.z-=m_fRadius;

	pBBox->m_vMax.x-=m_fRadius;
	pBBox->m_vMax.y-=m_fRadius;
	pBBox->m_vMax.z-=m_fRadius;
}

void BSphere::FromBBox(const BBox *pBBox)
{
	m_vCenter=pBBox->GetCenter();
	Adjust(pBBox->GetMax());
}

bool BSphere::IsBoxInside(const BBox *pBBox) const
{
	const Vector3& vMin(pBBox->GetMin());
	const Vector3& vMax(pBBox->GetMax());

	if((m_vCenter.x + m_fRadius) < vMin.x || (m_vCenter.x-m_fRadius) > vMax.x) return false;
	if((m_vCenter.y + m_fRadius) < vMin.y || (m_vCenter.y-m_fRadius) > vMax.y) return false;
	if((m_vCenter.z + m_fRadius) < vMin.z || (m_vCenter.z-m_fRadius) > vMax.z) return false;

	return true;
}

}