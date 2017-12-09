
#include "Transformation.h"

namespace Core
{

void Transformation::Initialize()
{
	m_vPosition.x=0.0f;
	m_vPosition.y=0.0f;
	m_vPosition.z=0.0f;

	m_qRotation=Quaternion();
	
	m_vScale.x=1.0f;
	m_vScale.y=1.0f;
	m_vScale.z=1.0f;
	
	m_bHasChanged=true;
}

const Transformation& Transformation::operator=(const Transformation& Trans)
{
	m_vPosition=Trans.m_vPosition;
	m_qRotation=Trans.m_qRotation;
	m_vScale=Trans.m_vScale;

	m_pBBox=Trans.m_pBBox;
	m_pTransBBox=Trans.m_pTransBBox;

	m_pBSphere=Trans.m_pBSphere;
	m_pTransBSphere=Trans.m_pTransBSphere;
	
	m_bHasChanged=true;
	
	return *this;
}

void Transformation::SetPosition(const Vector3& vPos)
{
	m_vPosition=vPos;
	m_bHasChanged=true;
}

void Transformation::SetRotation(const Vector3& vRot)
{
	m_qRotation.BuildEuler(vRot);
	m_bHasChanged=true;
}

void Transformation::SetRotation(const Quaternion& qRot)
{
	m_qRotation=qRot;
	m_bHasChanged=true;
}

void Transformation::SetScale(const Vector3& vScale)
{
	m_vScale=vScale;
	m_bHasChanged=true;
}

void Transformation::Translate(const Vector3& vTrans)
{
	m_vPosition+=vTrans;
	m_bHasChanged=true;
}

void Transformation::Rotate(const Vector3& vRot)
{
	Quaternion qRot;
	qRot.BuildEuler(vRot);
	m_qRotation*=qRot;
	m_bHasChanged=true;
}

void Transformation::Rotate(const Quaternion& qRot)
{
	m_qRotation*=qRot;
	m_bHasChanged=true;
}

void Transformation::Scale(const Vector3& vScale)
{
	m_vScale.x*=vScale.x;
	m_vScale.y*=vScale.y;
	m_vScale.z*=vScale.z;
	
	m_bHasChanged=true;
}

void Transformation::RebuildMatrix()
{
	if(m_bHasChanged || m_bAnim)
	{
		Matrix4 mRot,mScl;
		m_qRotation.GetMatrix(m_mWorld);
		mScl.CreateScale(m_vScale.x,m_vScale.y,m_vScale.z);
		
		m_mWorld*=mScl;
		m_mWorld.m[3][0]+=m_vPosition.x;
		m_mWorld.m[3][1]+=m_vPosition.y;
		m_mWorld.m[3][2]+=m_vPosition.z;

		if(m_pBBox && m_pTransBBox)
		{
			Vector3 tmin(m_pBBox->GetMin() * m_mWorld);
			Vector3 tmax(m_pBBox->GetMax() * m_mWorld);
			Vector3 fmin,fmax;

			if(tmin.x < tmax.x) { fmin.x=tmin.x; fmax.x=tmax.x; } else { fmin.x=tmax.x; fmax.x=tmin.x; }
			if(tmin.y < tmax.y) { fmin.y=tmin.y; fmax.y=tmax.y; } else { fmin.y=tmax.y; fmax.y=tmin.y; }
			if(tmin.z < tmax.z) { fmin.z=tmin.z; fmax.z=tmax.z; } else { fmin.z=tmax.z; fmax.z=tmin.z; }

			m_pTransBBox->SetMin(fmin);
			m_pTransBBox->SetMax(fmax);
		}

		if(m_pBSphere && m_pTransBSphere)
		{
			m_pTransBSphere->SetCenter(m_vPosition);
			m_pTransBSphere->SetRadius(m_pBSphere->GetRadius() * m_vScale.x);
		}

		if(m_bBuildInverse)
			m_mWorld.Inverse(m_mInvWorld);
		
		m_bHasChanged=false;
	}
}

const Matrix4& Transformation::GetMatrix()
{
	RebuildMatrix();
	return m_mWorld;
}

const Matrix4& Transformation::GetInverse()
{
	RebuildMatrix();
	return m_mInvWorld;
}

void Transformation::GetMatrices(Core::Matrix4 &mWorld, Core::Matrix4 &mInv)
{
	RebuildMatrix();
	mWorld=m_mWorld;
	mInv=m_mInvWorld;
}

}
