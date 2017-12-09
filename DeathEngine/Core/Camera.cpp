
#include "Camera.h"
#include "../MyAssert.h"
#include <d3dx9.h>

namespace Core
{

Camera::Camera() : m_bHasChanged(true), m_bTargetEnable(false), m_Type(CAM_PERSPECTIVE),
	m_fZMin(0.1f),m_fZMax(100.0f),m_fXCenter(320.0f),m_fYCenter(240.0f),m_fZoom(1.0f),
	m_iFOV(90), m_vPosition(Vector3::NullVector), m_vRotation(Vector3::NullVector),
	m_qRotation(Vector3::YAxisVector,0.0f),m_vXAxis(Vector3::XAxisVector),
	m_vYAxis(Vector3::YAxisVector),m_mProjection(Matrix4::Identity),m_mView(Matrix4::Identity),
	m_mViewProj(Matrix4::Identity), m_bProjHasChanged(true)
{
}

Camera::Camera(const Camera& cam) : m_bHasChanged(cam.m_bHasChanged), m_bTargetEnable(cam.m_bTargetEnable),
	m_Type(cam.m_Type),m_fZMin(cam.m_fZMin),m_fZMax(cam.m_fZMax), m_fXCenter(cam.m_fXCenter),
	m_fYCenter(cam.m_fYCenter),m_fZoom(cam.m_fZoom), m_iFOV(cam.m_iFOV),
	m_vPosition(cam.m_vPosition),m_vRotation(cam.m_vRotation), m_qRotation(cam.m_qRotation),
	m_vXAxis(cam.m_vXAxis),m_vYAxis(cam.m_vYAxis),m_mProjection(cam.m_mProjection),
	m_mView(cam.m_mView),m_mViewProj(cam.m_mView), m_Frustrum(cam.m_Frustrum),
	m_bProjHasChanged(true)
{
}

void Camera::Clear()
{
	m_bHasChanged=true;
	m_bTargetEnable=true;
	m_Type=CAM_PERSPECTIVE;
	m_fZMin=0.1f;
	m_fZMax=100.0f;
	m_fXCenter=320.0f;
	m_fYCenter=240.0f;
	m_fZoom=1.0f;
	m_iFOV=90;
	m_vPosition=Vector3::NullVector;
	m_vRotation=Vector3::NullVector;
	m_qRotation=Quaternion(Vector3::YAxisVector,0.0f);
	m_vXAxis=Vector3::XAxisVector;
	m_vYAxis=Vector3::YAxisVector;
	m_mProjection=Matrix4::Identity;
	m_mView=Matrix4::Identity;
	m_mViewProj=Matrix4::Identity;
	m_bProjHasChanged=true;
}

void Camera::Initialize(CameraType _Type, const float fZMin, const float fZMax,
						const float fXCenter, const float fYCenter,
						const int iFOV, const float fZoom)
{
	MYASSERT(_Type >= 0 && _Type < CAM_COUNT,"Mauvais type de camera",false);
	MYASSERT(fZMin < fZMax && fZMin > 0.0f && iFOV > 0 && iFOV < 180 && fZoom > 0.0f,"Parametres de la camera incorrects",true);

	m_Type=_Type;
	m_fZMin=fZMin; m_fZMax=fZMax;
	m_fXCenter=fXCenter; m_fYCenter=fYCenter;
	m_iFOV=iFOV;
	m_fZoom=fZoom;
	m_bHasChanged=true;
	m_bProjHasChanged=true;
}

void Camera::SetType(CameraType _Type)
{
	MYASSERT(_Type >= 0 && _Type < CAM_COUNT,"Mauvais type de camera",false);

	m_bHasChanged=true;
	m_bProjHasChanged=true;
	m_Type=_Type;
}

void Camera::SetZRange(float fZMin, float fZMax)
{
	MYASSERT(fZMin < fZMax && fZMin > 0.0f,"Mauvais range pour la camera",true);
	m_bHasChanged=true;
	m_bProjHasChanged=true;
	m_fZMin=fZMin; m_fZMax=fZMax;
}

void Camera::SetZMin(float fZMin)
{
	MYASSERT(fZMin < m_fZMax && fZMin > 0.0f,"Mauvais range pour la camera",false);
	m_fZMin=fZMin;
	m_bProjHasChanged=true;
	m_bHasChanged=true;
}

void Camera::SetZMax(float fZMax)
{
	MYASSERT(m_fZMin < fZMax,"Mauvais range pour la camera",false);
	m_fZMax=fZMax;
	m_bProjHasChanged=true;
	m_bHasChanged=true;
}

void Camera::SetFOV(int iFOV)
{
	MYASSERT(iFOV > 0 && iFOV < 180, "Mauvaise valeur pour le fov de la camera",true);
	m_iFOV=iFOV;
	m_bProjHasChanged=true;
	m_bHasChanged=true;
}

void Camera::SetZoom(float fZoom)
{
	MYASSERT(fZoom > 0.0f,"Le zoom de la camera doit etre > a 0",true);
	m_fZoom=fZoom;
	m_bProjHasChanged=true;
	m_bHasChanged=true;
}

void Camera::SetPosition(const Vector3 &vPos)
{
	m_vPosition=vPos;

	m_bHasChanged=true;
}

void Camera::SetTarget(const Vector3 &vPos)
{
	m_vTarget=vPos;
	m_bTargetEnable=true;
	m_bHasChanged=true;
}

void Camera::SetRotation(const Vector3& vRot)
{
	m_vRotation=vRot;
	m_qRotation.BuildEuler(m_vRotation);
	m_bTargetEnable=false;
	m_bHasChanged=true;
}

void Camera::SetRotation(const Quaternion& qRot)
{
	m_qRotation=qRot;
	m_bTargetEnable=false;
	m_bHasChanged=true;
}

void Camera::GetMatrices(Matrix4 &mView, Matrix4 &mProj, Matrix4 &mViewProj)
{
	if(m_bHasChanged)
		RebuildMatrices();

	mView=m_mView;
	mProj=m_mProjection;
	mViewProj=m_mViewProj;
}

const Matrix4& Camera::GetViewMatrix()
{
	if(m_bHasChanged)
		RebuildMatrices();

	return m_mView;
}

const Matrix4& Camera::GetProjectionMatrix()
{
	if(m_bProjHasChanged)
		RebuildMatrices();

	return m_mProjection;
}

const Matrix4& Camera::GetViewProjMatrix()
{
	if(m_bHasChanged)
		RebuildMatrices();

	return m_mViewProj;
}

void Camera::RebuildMatrices()
{
	if(m_bProjHasChanged)
	{
		const float Q=m_fZMax/(m_fZMax - m_fZMin);
		const float h=(float)(cosf(float(m_iFOV) * DEGTORAD * 0.5f ) / sinf(float(m_iFOV) * DEGTORAD * 0.5f));
		const float w=m_fXCenter*h / m_fYCenter;
		const float invz=1.0f / m_fZMax;
		const float invx=m_fZoom / m_fXCenter;
		const float invy=m_fZoom / m_fYCenter;

		switch(m_Type)
		{
			case CAM_PERSPECTIVE:
				m_mProjection.m[0][0]=2*m_fZMin / w; m_mProjection.m[0][1]=0.0f			; m_mProjection.m[0][2]=0.0f	   ; m_mProjection.m[0][3]=0.0f;
				m_mProjection.m[1][0]=0.0f		   ; m_mProjection.m[1][1]=2*m_fZMin / h; m_mProjection.m[1][2]=0.0f	   ; m_mProjection.m[1][3]=0.0f;
				m_mProjection.m[2][0]=0.0f		   ; m_mProjection.m[2][1]=0.0f			; m_mProjection.m[2][2]=Q		   ; m_mProjection.m[2][3]=1.0f;
				m_mProjection.m[3][0]=0.0f		   ; m_mProjection.m[3][1]=0.0f			; m_mProjection.m[3][2]=-Q*m_fZMin; m_mProjection.m[3][3]=0.0f;
				break;

			case CAM_TOP:
				m_mProjection.m[0][0]=invx; m_mProjection.m[1][0]=0.0f; m_mProjection.m[2][0]=0.0f ; m_mProjection.m[3][0]=0.0f;
				m_mProjection.m[0][1]=0.0f; m_mProjection.m[1][1]=0.0f; m_mProjection.m[2][1]=-invy; m_mProjection.m[3][1]=0.0f;
				m_mProjection.m[0][2]=0.0f; m_mProjection.m[1][2]=invz; m_mProjection.m[2][2]=0.0f ; m_mProjection.m[3][2]=0.0f;
				m_mProjection.m[0][3]=0.0f; m_mProjection.m[1][3]=0.0f; m_mProjection.m[2][3]=0.0f ; m_mProjection.m[3][3]=1.0f;
				break;

			case CAM_BOTTOM:
				m_mProjection.m[0][0]=invx; m_mProjection.m[1][0]=0.0f; m_mProjection.m[2][0]=0.0f;	m_mProjection.m[3][0]=0.0f;
				m_mProjection.m[0][1]=0.0f; m_mProjection.m[1][1]=0.0f; m_mProjection.m[2][1]=invy;	m_mProjection.m[3][1]=0.0f;
				m_mProjection.m[0][2]=0.0f; m_mProjection.m[1][2]=invz; m_mProjection.m[2][2]=0.0f;	m_mProjection.m[3][2]=0.0f;
				m_mProjection.m[0][3]=0.0f; m_mProjection.m[1][3]=0.0f; m_mProjection.m[2][3]=0.0f;	m_mProjection.m[3][3]=1.0f;
				break;

			case CAM_FRONT:
				m_mProjection.m[0][0]=invx; m_mProjection.m[1][0]=0.0f; m_mProjection.m[2][0]=0.0f;	m_mProjection.m[3][0]=0.0f;
				m_mProjection.m[0][1]=0.0f; m_mProjection.m[1][1]=invy; m_mProjection.m[2][1]=0.0f;	m_mProjection.m[3][1]=0.0f;
				m_mProjection.m[0][2]=0.0f; m_mProjection.m[1][2]=0.0f; m_mProjection.m[2][2]=invz;	m_mProjection.m[3][2]=0.0f;
				m_mProjection.m[0][3]=0.0f; m_mProjection.m[1][3]=0.0f; m_mProjection.m[2][3]=0.0f;	m_mProjection.m[3][3]=1.0f;
				break;

			case CAM_BACK:
				m_mProjection.m[0][0]=-invx; m_mProjection.m[1][0]=0.0f; m_mProjection.m[2][0]=0.0f ; m_mProjection.m[3][0]=0.0f;
				m_mProjection.m[0][1]=0.0f ; m_mProjection.m[1][1]=invy; m_mProjection.m[2][1]=0.0f ; m_mProjection.m[3][1]=0.0f;
				m_mProjection.m[0][2]=0.0f ; m_mProjection.m[1][2]=0.0f; m_mProjection.m[2][2]=-invz; m_mProjection.m[3][2]=0.0f;
				m_mProjection.m[0][3]=0.0f ; m_mProjection.m[1][3]=0.0f; m_mProjection.m[2][3]=0.0f ; m_mProjection.m[3][3]=1.0f;
				break;

			case CAM_LEFT:
				m_mProjection.m[0][0]=0.0f; m_mProjection.m[1][0]=0.0f; m_mProjection.m[2][0]=invx; m_mProjection.m[3][0]=0.0f;
				m_mProjection.m[0][1]=0.0f; m_mProjection.m[1][1]=invy; m_mProjection.m[2][1]=0.0f; m_mProjection.m[3][1]=0.0f;
				m_mProjection.m[0][2]=invz; m_mProjection.m[1][2]=0.0f; m_mProjection.m[2][2]=0.0f; m_mProjection.m[3][2]=0.0f;
				m_mProjection.m[0][3]=0.0f; m_mProjection.m[1][3]=0.0f; m_mProjection.m[2][3]=0.0f; m_mProjection.m[3][3]=1.0f;
				break;

			case CAM_RIGHT:
				m_mProjection.m[0][0]=0.0f; m_mProjection.m[1][0]=0.0f; m_mProjection.m[2][0]=-invx; m_mProjection.m[3][0]=0.0f;
				m_mProjection.m[0][1]=0.0f; m_mProjection.m[1][1]=invy; m_mProjection.m[2][1]=0.0f ; m_mProjection.m[3][1]=0.0f;
				m_mProjection.m[0][2]=invz; m_mProjection.m[1][2]=0.0f; m_mProjection.m[2][2]=0.0f ; m_mProjection.m[3][2]=0.0f;
				m_mProjection.m[0][3]=0.0f; m_mProjection.m[1][3]=0.0f; m_mProjection.m[2][3]=0.0f ; m_mProjection.m[3][3]=1.0f;
				break;
		}

		m_bProjHasChanged=false;
	}

	RebuildViewMatrix();

	m_vXAxis=Vector3(m_mView.m[0][0],m_mView.m[1][0],m_mView.m[2][0]);
	m_vYAxis=Vector3(m_mView.m[0][1],m_mView.m[1][1],m_mView.m[2][1]);
	m_vXAxis.Normalize();
	m_vYAxis.Normalize();

	m_mViewProj=m_mView * m_mProjection;
	m_Frustrum.Create(m_mViewProj);
	m_Frustrum.SetPosition(m_vPosition);
	m_bHasChanged=false;
}

void Camera::RebuildViewMatrix()
{
	D3DXMATRIX view;

	if(m_Type==CAM_PERSPECTIVE)
	{
		D3DXVECTOR3 from,to,up;

		from.x=m_vPosition.x;
		from.y=m_vPosition.y;
		from.z=m_vPosition.z;

		if(m_bTargetEnable)
		{
			up.x=0.0f;
			up.y=1.0f;
			up.z=0.0f;

			to.x=m_vTarget.x;
			to.y=m_vTarget.y;
			to.z=m_vTarget.z;
		}
		else
		{
			Matrix4 mRot;
			m_qRotation.BuildMatrix(mRot);
			Vector3 vup(Vector3::YAxisVector * mRot);
			Vector3 vto(Vector3::ZAxisVector * mRot);

			up.x=vup.x; up.y=vup.y; up.z=vup.z;
			to.x=vto.x; to.y=vto.y; to.z=vto.z;
		}

		D3DXMatrixLookAtLH(&view,&from,&to,&up);

		m_mView.m[0][0]=view._11; m_mView.m[0][1]=view._12; m_mView.m[0][2]=view._13; m_mView.m[0][3]=view._14;
		m_mView.m[1][0]=view._21; m_mView.m[1][1]=view._22; m_mView.m[1][2]=view._23; m_mView.m[1][3]=view._24;
		m_mView.m[2][0]=view._31; m_mView.m[2][1]=view._32; m_mView.m[2][2]=view._33; m_mView.m[2][3]=view._34;
		m_mView.m[3][0]=view._41; m_mView.m[3][1]=view._42; m_mView.m[3][2]=view._43; m_mView.m[3][3]=view._44;
	}
	else
	{
		m_mView.CreateTranslate(-m_vPosition.x,-m_vPosition.y,-m_vPosition.z);

		switch(m_Type)
		{
			case CAM_TOP:
			case CAM_BOTTOM:
				m_mView.m[3][1]+=m_fZMax * 0.5f;
				break;

			case CAM_FRONT:
				m_mView.m[3][2]+=m_fZMax * 0.5f;
				break;

			case CAM_BACK:
				m_mView.m[3][2]-=m_fZMax * 0.5f;
				break;

			case CAM_LEFT:
			case CAM_RIGHT:
				m_mView.m[3][0]+=m_fZMax * 0.5f;
				break;

			default:
				break;
		}
	}

	m_mView.Inverse(m_mInvView);
}

void Camera::MakeRayFrom2D(int x, int y, Vector3& vOrigin, Vector3& vDir)
{
	if(m_bHasChanged) RebuildMatrices();

	if(m_Type==CAM_PERSPECTIVE)
	{
		const float fWidth=GetXCenter() * 2.0f;
		const float fHeight=GetYCenter() * 2.0f;

		Core::Vector3 v;
		v.x=((( 2.0f * ((float)x)) / fWidth) - 1.0f) / m_mProjection.m[0][0];
		v.y=-((( 2.0f * ((float)y)) / fHeight)- 1.0f) / m_mProjection.m[1][1];
		v.z=1.0f;

		vDir.x=v.x * m_mInvView.m[0][0] + v.y * m_mInvView.m[1][0] + v.z * m_mInvView.m[2][0];
		vDir.y=v.x * m_mInvView.m[0][1] + v.y * m_mInvView.m[1][1] + v.z * m_mInvView.m[2][1];
		vDir.z=v.x * m_mInvView.m[0][2] + v.y * m_mInvView.m[1][2] + v.z * m_mInvView.m[2][2];

		vOrigin.x=m_mInvView.m[3][0];
		vOrigin.y=m_mInvView.m[3][1];
		vOrigin.z=m_mInvView.m[3][2];

		vDir.Normalize();
	}
	else
	{
		vOrigin=m_vPosition;

		const float izoom=KINV(m_fZoom);
		const float xf=((float)x - m_fXCenter)*izoom;
		const float yf=((float)y - m_fYCenter)*izoom;

		switch(m_Type)
		{
			case CAM_TOP:
				vDir=Core::Vector3::YAxisVector.Negative();
				vOrigin.x+=xf;
				vOrigin.z+=yf;
				break;

			case CAM_BOTTOM:
				vDir=Core::Vector3::YAxisVector;
				vOrigin.x+=xf;
				vOrigin.z-=yf;
				break;

			case CAM_LEFT:
				vDir=Core::Vector3::XAxisVector;
				vOrigin.z+=xf;
				vOrigin.y-=yf;
				break;

			case CAM_RIGHT:
				vDir=Core::Vector3::XAxisVector.Negative();
				vOrigin.z-=xf;
				vOrigin.y-=yf;
				break;

			case CAM_FRONT:
				vDir=Core::Vector3::ZAxisVector;
				vOrigin.x+=xf;
				vOrigin.y-=yf;
				break;

			case CAM_BACK:
				vDir=Core::Vector3::ZAxisVector.Negative();
				vOrigin.x-=xf;
				vOrigin.y-=yf;
				break;
		}
	}
}

}