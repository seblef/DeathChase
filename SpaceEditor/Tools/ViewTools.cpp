
#include "ViewTools.h"


bool ToolCamZoom::Start(ICamera *pCam, int x, int y, bool bLockX, bool bLockY, bool bLockZ)
{
	if(!pCam) return false;
	else
	{
		m_pCamera=pCam;
		return true;
	}
}

int ToolCamZoom::MouseMove(const int dx, const int dy)
{
	if(m_pCamera && m_pCamera->GetType()!=CAM_PERSPECTIVE)
	{
		float fZoom=m_pCamera->GetZoom();
		fZoom*=dy < 0 ? 1.1f : (1.0f / 1.1f);
		m_pCamera->SetZoom(fZoom);

		return TR_REDRAWMAINVIEW;
	}
	else
		return 0;
}



bool ToolCamMove::Start(ICamera *pCam, int x, int y, bool bLockX, bool bLockY, bool bLockZ)
{
	if(!pCam) return false;
	else
	{
		m_pCamera=pCam;
		return true;
	}
}

int ToolCamMove::MouseMove(const int dx, const int dy)
{
	if(!m_pCamera) return 0;
	if(m_pCamera->GetType()==CAM_PERSPECTIVE)
	{
		const Core::Vector3 vTrans(m_pCamera->GetXAxis() * ((float)dx * -0.1f) +
			m_pCamera->GetYAxis() * ((float)dy * 0.1f));
		m_pCamera->SetTarget(m_pCamera->GetTarget() + vTrans);
		m_pCamera->SetPosition(m_pCamera->GetPosition() + vTrans);
	}
	else
	{
		const float fIZoom=m_pCamera->GetZoom()!=0.0f ? 1.0f / m_pCamera->GetZoom() : 0.0f;
		Core::Vector3 vTrans(Core::Vector3::NullVector);

		switch(m_pCamera->GetType())
		{
			case CAM_FRONT:
				vTrans.x=((float)-dx) * fIZoom;
				vTrans.y=((float) dy) * fIZoom;
				break;

			case CAM_TOP:
				vTrans.x=((float)-dx) * fIZoom;
				vTrans.z=((float)-dy) * fIZoom;
				break;

			case CAM_LEFT:
				vTrans.y=((float) dy) * fIZoom;
				vTrans.z=((float)-dx) * fIZoom;
				break;

			case CAM_BACK:
				vTrans.x=((float) dx) * fIZoom;
				vTrans.y=((float) dy) * fIZoom;
				break;

			case CAM_BOTTOM:
				vTrans.x=((float)-dx) * fIZoom;
				vTrans.z=((float) dy) * fIZoom;
				break;

			case CAM_RIGHT:
				vTrans.y=((float) dy) * fIZoom;
				vTrans.z=((float) dx) * fIZoom;
				break;

			default:
				break;
		}

		m_pCamera->SetPosition(m_pCamera->GetPosition() + vTrans);
	}

	return TR_REDRAWMAINVIEW;
}

bool ToolCamRotate::Start(ICamera *pCam, int x, int y, bool bLockX, bool bLockY, bool bLockZ)
{
	if(!pCam) return false;
	else
	{
		m_pCamera=pCam;
		return true;
	}
}

int ToolCamRotate::MouseMove(const int dx, const int dy)
{
	if(m_pCamera && m_pCamera->GetType()==CAM_PERSPECTIVE)
	{
		Core::Vector3 vFromTarget(m_pCamera->GetPosition() - m_pCamera->GetTarget());
		const Core::Quaternion qX(m_pCamera->GetXAxis(),(float)( dy)*0.01f);
		const Core::Quaternion qY(m_pCamera->GetYAxis(),(float)( dx)*0.01f);
		Core::Quaternion qRot(qY*qX);
		vFromTarget=qRot * vFromTarget;
		
		const float fdx=vFromTarget.y==0.0f ? 0.0f : fabs(vFromTarget.x / vFromTarget.y);
		const float fdz=vFromTarget.y==0.0f ? 0.0f : fabs(vFromTarget.z / vFromTarget.y);

		if(fdx < 0.02f && fdz < 0.02f) return 0;

		m_pCamera->SetPosition(m_pCamera->GetTarget() + vFromTarget);
		return TR_REDRAWMAINVIEW;
	}
	else
		return 0;
}

bool ToolCamZMove::Start(ICamera *pCam, int x, int y, bool bLockX, bool bLockY, bool bLockZ)
{
	if(!pCam) return false;
	else
	{
		m_pCamera=pCam;
		return true;
	}
}

int ToolCamZMove::MouseMove(const int dx, const int dy)
{
	if(m_pCamera && m_pCamera->GetType()==CAM_PERSPECTIVE && dy!=0)
	{
		Core::Vector3 vDir(m_pCamera->GetPosition() - m_pCamera->GetTarget());
		vDir.Normalize();
		vDir*=((float)(-dy)) * 0.5f;

//		if(dy > 0) vDir=vDir.Negative();

		m_pCamera->SetTarget(m_pCamera->GetTarget() + vDir);
		m_pCamera->SetPosition(m_pCamera->GetPosition() + vDir);
//		vFromTarget*=dy < 0 ? 1.05f : (1.0f / 1.05f);
//		m_pCamera->SetPosition(m_pCamera->GetTarget() + vFromTarget);

		return TR_REDRAWMAINVIEW;
	}
	else
		return 0;
}