
#include "GameCamera.h"
#include "Map.h"
#include "../Core/Sound.h"
#include <d3dx9.h>

#define TRACE_OFFSET				3

namespace Game
{

void GameCamera::ActiveTurnAround(const Core::Vector3& vTarget,
									const float fHeight,
									const float fDist,
									const float fSpeed)
{
	m_vTarget=vTarget;
	m_fHeight=fHeight;
	m_fDistance=fDist;
	m_fSpeed=fSpeed;
	m_fCurrentAngle=0.0f;
	m_iState=CAMSTATE_TURNAROUND;
	m_bTargetEnable=true;
}

void GameCamera::ActiveFollowShip()
{
	m_iCurrentTrace=-1;
	m_iState=CAMSTATE_FOLLOWSHIP;
	m_bTargetEnable=true;
}

void GameCamera::ActiveFollowTarget(const Core::Vector3& vPos)
{
	m_iState=CAMSTATE_FOLLOWTARGET;
	m_vPosition=vPos;
	m_bTargetEnable=true;
}

void GameCamera::UpdateTurnAround(const float fTime)
{
	m_fCurrentAngle+=fTime*m_fSpeed;

	Core::Matrix4 mRot;
	mRot.CreateRotateY((int)m_fCurrentAngle);
	m_vPosition=Core::Vector3(0.0f,0.0f,m_fDistance);
	m_vPosition*=mRot;
	m_vPosition.y+=m_fHeight+1.0f;
	m_vPosition+=m_vTarget;

	float fSize=(float)g_Map.GetHeightmap()->GetSize();

	if(m_vPosition.x >= 0.0f && m_vPosition.x < (fSize-1.0f) &&
	   m_vPosition.z >= 0.0f && m_vPosition.z < (fSize-1.0f))
	{
		float fHeight=g_Map.GetHeightmap()->GetHeight((int)m_vPosition.x,(int)m_vPosition.z);

		if(m_vPosition.y < fHeight)
			m_vPosition.y=fHeight+1.0f;
	}
	else if(m_vPosition.y < g_Map.GetWater()->GetHeight())
		m_vPosition.y=g_Map.GetWater()->GetHeight() + 5.0f;

	m_bHasChanged=true;
}

void GameCamera::UpdateFollowShip(const float fTime,
								  const Core::Vector3& vShipPos,
								  const Core::Quaternion& qShipRot)
{
	m_fTime+=fTime;
	if(m_fTime >= 0.1f)
		m_fTime-=0.1f;

	m_vTarget=vShipPos;

	if(m_iCurrentTrace==-1 || m_iTraceCount < 1)
	{
		m_vPosition=m_vShipPosition[0];
		m_vUp=Core::Vector3::YAxisVector;
		return;
	}

	int id2=m_iCurrentTrace - TRACE_OFFSET;
	if(id2 > MAX_TRACE_CAMERA) id2-=MAX_TRACE_CAMERA;

	int id3=m_iCurrentTrace - TRACE_OFFSET + 1;
	if(id3 > MAX_TRACE_CAMERA) id3-=MAX_TRACE_CAMERA;

	if(id2 < 0) id2+=MAX_TRACE_CAMERA;
	if(id3 < 0) id3+=MAX_TRACE_CAMERA;

	const float t=m_fTime / 0.1f;

	m_vPosition=m_vShipPosition[id2] + (m_vShipPosition[id3] - m_vShipPosition[id2]) * t;

	Core::Matrix4 mRot2;
	Core::Matrix4 mRot3;

	m_qShipRotation[id2].GetMatrix(mRot2);
	m_qShipRotation[id3].GetMatrix(mRot3);

	Core::Vector3 vUp2(Core::Vector3::YAxisVector * mRot2);
	Core::Vector3 vUp3(Core::Vector3::YAxisVector * mRot3);

	m_vUp=vUp2 + (vUp3 - vUp2) * t;
	m_vUp.Normalize();

	m_bHasChanged=true;
}

void GameCamera::AddTrajectoryPoint(const Core::Vector3& vPos, const Core::Quaternion& qRot)
{
	if(m_iCurrentTrace==-1)
	{
		int i;
		for(i=0;i<MAX_TRACE_CAMERA;i++)
		{
			m_vShipPosition[i]=vPos;
			m_qShipRotation[i]=qRot;
		}
		m_iCurrentTrace=0;
	}
	else
	{
		m_iCurrentTrace++;
		if(m_iCurrentTrace >= MAX_TRACE_CAMERA)
			m_iCurrentTrace=0;

		m_vShipPosition[m_iCurrentTrace]=vPos;
		m_qShipRotation[m_iCurrentTrace]=qRot;
	}

	if(m_iTraceCount < 100)
		m_iTraceCount++;
}

void GameCamera::UpdateFollowTarget(const Core::Vector3& vTargetPos)
{
	m_vTarget=vTargetPos;
	m_bHasChanged=true;
}

void GameCamera::RebuildViewMatrix()
{
	if(m_Type!=CAM_PERSPECTIVE)
	{
		Camera::RebuildViewMatrix();
		return;
	}

	D3DXVECTOR3 from,to,up;
	D3DXMATRIX view;

	from.x=m_vPosition.x;
	from.y=m_vPosition.y;
	from.z=m_vPosition.z;

	if(m_iState==CAMSTATE_FOLLOWSHIP)
	{
		up.x=m_vUp.x;
		up.y=m_vUp.y;
		up.z=m_vUp.z;
	}
	else
	{
		up.x=0.0f;
		up.y=1.0f;
		up.z=0.0f;
	}

	to.x=m_vTarget.x;
	to.y=m_vTarget.y;
	to.z=m_vTarget.z;

	D3DXMatrixLookAtLH(&view,&from,&to,&up);

	m_mView.m[0][0]=view._11; m_mView.m[0][1]=view._12; m_mView.m[0][2]=view._13; m_mView.m[0][3]=view._14;
	m_mView.m[1][0]=view._21; m_mView.m[1][1]=view._22; m_mView.m[1][2]=view._23; m_mView.m[1][3]=view._24;
	m_mView.m[2][0]=view._31; m_mView.m[2][1]=view._32; m_mView.m[2][2]=view._33; m_mView.m[2][3]=view._34;
	m_mView.m[3][0]=view._41; m_mView.m[3][1]=view._42; m_mView.m[3][2]=view._43; m_mView.m[3][3]=view._44;

	g_Sound.GetListener()->SetPosition(m_vPosition);
	g_Sound.GetListener()->SetOrientation(Core::Vector3(to.x-from.x,to.y-from.y,to.z-from.z), Core::Vector3(up.x,up.y,up.z));
	g_Sound.GetListener()->SetVelocity(Core::Vector3::NullVector);
}

}