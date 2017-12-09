
#include "ObjectTools.h"
#include "../../DeathEngine/Core/Plane.h"
#include "../../DeathEditor/Interfaces.h"

#include "../../DeathEngine/Export/IMeshMgr.h"
#include "../../DeathEngine/Export/IMesh.h"
#include "../../DeathEngine/Export/IRenderer.h"
#include "../../DeathEngine/Export/IShaderMgr.h"
#include "../../DeathEngine/Export/IMaterialMgr.h"
#include "../../DeathEngine/Export/IMaterial.h"

IToolObject::IToolObject() : m_pObj(0), m_pCam(0), m_iLastStartX(-10), m_iLastStartY(-10)
{
}

void IToolObject::End(int x, int y)
{
//	m_iLastStartX=x;
//	m_iLastStartY=y;
}

int IToolObject::MouseMove(const int dx, const int dy)
{
	m_iTotalMoveX+=dx;
	m_iTotalMoveY+=dy;

	return 0;
}

void IToolObject::DoSelection(int x, int y, ICamera *pCam)
{
	if(x >= m_iLastStartX-1 && x <= m_iLastStartX +1 &&
	  (y >= m_iLastStartY-1 && y <= m_iLastStartY +1))
	{
		if(m_iTotalMoveX >= -1 && m_iTotalMoveX <= 1 &&
		  (m_iTotalMoveY >= -1 && m_iTotalMoveY <= 1))
		{
			m_pObj=g_Selector.RotateSelection();
		}
		else
		{
			int iSelCount=g_Selector.Pick(x,y,pCam);
			if(iSelCount > 0 && m_pObj)
			{
				std::vector<Selector::SelectedItem> *pItems=g_Selector.GetSelection();
				std::vector<Selector::SelectedItem>::iterator i;
				bool bFound=false;

				for(i=pItems->begin();i!=pItems->end();i++)
					if(i->pObject==m_pObj)
					{
						bFound=true;
						break;
					}

				if(!bFound)
					m_pObj=g_Selector.GetSelected();
			}
			else
				m_pObj=g_Selector.GetSelected();
		}
	}
	else
	{
		m_iLastStartX=x;
		m_iLastStartY=y;

		g_Selector.Pick(x,y,pCam);
		m_pObj=g_Selector.GetSelected();
	}

	m_iTotalMoveX=0;
	m_iTotalMoveY=0;

	m_pCam=pCam;
}


//				Translation



bool ToolTranslate::Start(ICamera *pCam, int x, int y, bool bLockX, bool bLockY, bool bLockZ)
{
	if(m_bForced)
	{
		m_pCam=pCam;

		m_bForced=false;
		m_bCreateObject=true;
		g_Selector.ForceSelection(m_pObj);

		m_iLastStartX=x,
		m_iLastStartY=y;
		m_iTotalMoveX=0;
		m_iTotalMoveY=0;

		Core::Vector3 vOrigin,vDir;
		m_pCam->MakeRayFrom2D(m_iLastStartX+m_iTotalMoveX,m_iLastStartY+m_iTotalMoveY,vOrigin,vDir);

		if(m_pCam->GetType()==CAM_PERSPECTIVE)
			m_pObj->GetTransformation().SetPosition(vOrigin + vDir*10.0f);
		else
		{
			Core::Vector3 vPos(Core::Vector3::NullVector);
			switch(m_pCam->GetType())
			{
				case CAM_TOP:
				case CAM_BOTTOM:
					vPos.x=vOrigin.x;
					vPos.z=vOrigin.z;
					break;

				case CAM_FRONT:
				case CAM_BACK:
					vPos.x=vOrigin.x;
					vPos.y=vOrigin.y;
					break;

				case CAM_LEFT:
				case CAM_RIGHT:
					vPos.y=vOrigin.y;
					vPos.z=vOrigin.z;
					break;
			}

			m_pObj->GetTransformation().SetPosition(vPos);
		}

		m_pObj->GetTransformation().RebuildMatrix();
	}
	else
		DoSelection(x,y,pCam);

	if(!m_pObj)
		return false;

	m_bLocked[0]=bLockX;
	m_bLocked[1]=bLockY;
	m_bLocked[2]=bLockZ;

	return true;
}

int ToolTranslate::MouseMove(const int dx, const int dy)
{
	IToolObject::MouseMove(dx,dy);

	if(!m_pObj || !m_pCam) return 0;

	int iRet=m_bCreateObject ? (TR_CREATEOBJECT | TR_REDRAWMAINVIEW) : TR_REDRAWMAINVIEW;
	m_bCreateObject=false;

	if(m_pCam->GetType()==CAM_PERSPECTIVE)
	{
		Core::Plane plane;
		const Core::Vector3 xaxis(m_pCam->GetXAxis());
		const Core::Vector3 yaxis(m_pCam->GetYAxis());
		const Core::Vector3 pos(m_pObj->GetTransformation().GetPosition());
		plane.MakePlane(pos,pos+xaxis,pos+yaxis);

		Core::Vector3 vOrigin, vDir;
		const int x=m_iLastStartX + m_iTotalMoveX;
		const int y=m_iLastStartY + m_iTotalMoveY;
		m_pCam->MakeRayFrom2D(x,y,vOrigin,vDir);

		Core::Vector3 newpos;
		if(plane.IntersectLine(vOrigin,vDir,newpos))
		{
			Core::Vector3 translate(newpos - pos);

			if(m_bLocked[0]) translate.x=0.0f;
			if(m_bLocked[1]) translate.y=0.0f;
			if(m_bLocked[2]) translate.z=0.0f;

			m_pObj->GetTransformation().Translate(translate);
			m_pObj->GetTransformation().RebuildMatrix();

			return iRet;
		}
		else
			return 0;
	}
	else
	{
		Core::Vector3 v;
		const float _dx=(float)dx;
		const float _dy=(float)dy;
		const float izoom=KINV(m_pCam->GetZoom());

		switch(m_pCam->GetType())
		{
			case CAM_FRONT:
				v.x=_dx * izoom;
				v.y=_dy * izoom;
				v.z=0.0f;
				break;

			case CAM_BACK:
				v.x=-_dx * izoom;
				v.y=_dy*izoom;
				v.z=0.0f;
				break;

			case CAM_LEFT:
				v.x=0.0f;
				v.y=_dy*izoom;
				v.z=_dx*izoom;
				break;

			case CAM_RIGHT:
				v.x=0.0f;
				v.y=_dy*izoom;
				v.z=-_dx*izoom;
				break;

			case CAM_TOP:
				v.x=_dx*izoom;
				v.y=0.0f;
				v.z=-_dy*izoom;
				break;

			case CAM_BOTTOM:
				v.x=_dx*izoom;
				v.y=0.0f;
				v.z=_dy*izoom;
				break;
		}

		if(m_bLocked[0]) v.x=0.0f;
		if(m_bLocked[1]) v.y=0.0f;
		if(m_bLocked[2]) v.z=0.0f;

		m_pObj->GetTransformation().Translate(v);
		m_pObj->GetTransformation().RebuildMatrix();

		return iRet;
	}
}

void ToolTranslate::Draw()
{

}

void ToolTranslate::End(int x, int y)
{
	if(m_bForced)
	{
		delete m_pObj;
		m_pObj=0;
		m_bForced=false;
	}
}

void ToolTranslate::ForceObject(SObject *pObj)
{
	if(pObj)
	{
		m_pObj=pObj;
		m_bForced=true;
	}
}


// ********************************** ToolRotate *********************************

bool ToolRotate::Start(ICamera *pCam, int x, int y, bool bLockX, bool bLockY, bool bLockZ)
{
	DoSelection(x,y,pCam);

	if(!m_pObj) return false;

	m_bLocked[0]=bLockX;
	m_bLocked[1]=bLockY;
	m_bLocked[2]=bLockZ;

	return true;
}

int ToolRotate::MouseMove(const int dx, const int dy)
{
	IToolObject::MouseMove(dx,dy);

	if(!m_pObj) return 0;

	const float dx_rad=((float)dx) * DEGTORAD * 0.5f;
	const float dy_rad=((float)dy) * DEGTORAD * 0.5f;

	Core::Quaternion qRotA;
	Core::Quaternion qRotB;

	if(m_pCam->GetType()==CAM_PERSPECTIVE)
	{
		const Core::Vector3 xaxis(m_pCam->GetXAxis());
		const Core::Vector3 yaxis(m_pCam->GetYAxis());

		qRotA.BuildAxisAngle(xaxis,dy_rad);
		qRotB.BuildAxisAngle(yaxis,dx_rad);

		Core::Quaternion qFinal;

		if(m_bLocked[0] || m_bLocked[1] || m_bLocked[2])
		{
			Core::Matrix4 mRot;
			(qRotA*qRotB).GetMatrix(mRot);
			
			if(m_bLocked[0])
			{
				mRot.m[0][0]=1.0f; mRot.m[1][0]=0.0f; mRot.m[2][0]=0.0f; mRot.m[3][0]=0.0f;
			}

			if(m_bLocked[1])
			{
				mRot.m[0][1]=1.0f; mRot.m[1][1]=0.0f; mRot.m[2][1]=0.0f; mRot.m[3][1]=0.0f;
			}

			if(m_bLocked[2])
			{
				mRot.m[0][2]=1.0f; mRot.m[1][2]=0.0f; mRot.m[2][2]=0.0f; mRot.m[3][2]=0.0f;
			}

			qFinal.BuildMatrix(mRot);
		}
		else
			qFinal=qRotA*qRotB;

		m_pObj->GetTransformation().Rotate(qFinal);
	}
	else
	{
		switch(m_pCam->GetType())
		{
			case CAM_FRONT:
				if(!m_bLocked[0]) qRotA.BuildAxisAngle(Core::Vector3::XAxisVector,dy_rad);
				if(!m_bLocked[1]) qRotB.BuildAxisAngle(Core::Vector3::YAxisVector,dx_rad);
				break;

			case CAM_TOP:
				if(!m_bLocked[0]) qRotA.BuildAxisAngle(Core::Vector3::XAxisVector,dy_rad);
				if(!m_bLocked[2]) qRotB.BuildAxisAngle(Core::Vector3::ZAxisVector,dx_rad);
				break;

			case CAM_LEFT:
				if(!m_bLocked[1]) qRotA.BuildAxisAngle(Core::Vector3::YAxisVector,dx_rad);
				if(!m_bLocked[2]) qRotB.BuildAxisAngle(Core::Vector3::ZAxisVector,dy_rad);
				break;

			case CAM_BACK:
				if(!m_bLocked[0]) qRotA.BuildAxisAngle(Core::Vector3::XAxisVector,dy_rad);
				if(!m_bLocked[1]) qRotB.BuildAxisAngle(Core::Vector3::YAxisVector,dx_rad);
				break;

			case CAM_BOTTOM:
				if(!m_bLocked[0]) qRotA.BuildAxisAngle(Core::Vector3::XAxisVector,dy_rad);
				if(!m_bLocked[2]) qRotB.BuildAxisAngle(Core::Vector3::ZAxisVector,dx_rad);
				break;

			case CAM_RIGHT:
				if(!m_bLocked[1]) qRotA.BuildAxisAngle(Core::Vector3::YAxisVector,dx_rad);
				if(!m_bLocked[2]) qRotB.BuildAxisAngle(Core::Vector3::ZAxisVector,dy_rad);
				break;
		}

		if(qRotA==Core::Quaternion())
		{
			if(qRotB==Core::Quaternion())
				return 0;
			else
				m_pObj->GetTransformation().Rotate(qRotB);
		}
		else
		{
			if(qRotB==Core::Quaternion())
				m_pObj->GetTransformation().Rotate(qRotA);
			else
				m_pObj->GetTransformation().Rotate(qRotA*qRotB);
		}
	}

	m_pObj->GetTransformation().RebuildMatrix();
	return TR_REDRAWMAINVIEW;
}


// *********************************** ToolScale *************************************

bool ToolScale::Start(ICamera *pCam, int x, int y, bool bLockX, bool bLockY, bool bLockZ)
{
	DoSelection(x,y,pCam);
	return m_pObj!=0;
}

int ToolScale::MouseMove(const int dx, const int dy)
{
	IToolObject::MouseMove(dx,dy);

	if(!m_pObj || dy==0) return 0;

	const float s=dy > 0 ? 1.1f : 1.0f/1.1f;
	m_pObj->GetTransformation().Scale(Core::Vector3(s,s,s));
	m_pObj->GetTransformation().RebuildMatrix();

	return TR_REDRAWMAINVIEW;
}

