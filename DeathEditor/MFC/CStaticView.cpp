
#include "CStaticView.h"
#include "../../DeathEngine/Export/ICamera.h"
#include "../../DeathEngine/Export/IRenderer.h"
#include "../../DeathEngine/Export/IFactory.h"
#include "../../DeathEngine/Export/IMeshMgr.h"
#include "../../DeathEngine/Export/IMesh.h"
#include "../Interfaces.h"
#include "../resource.h"

BEGIN_MESSAGE_MAP( CStaticMeshView, CStatic)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_PAINT()
END_MESSAGE_MAP( )

CStaticMeshView::CStaticMeshView() : m_bDragging(false),m_pCamera(0)
{
	m_pCamera=(ICamera *)g_Factory->CreateObject(EOBJ_CAMERA);
}

CStaticMeshView::~CStaticMeshView()
{
	g_Factory->ReleaseObject(m_pCamera);
}

void CStaticMeshView::Draw()
{
	g_Renderer->SetView(m_hWnd);
	g_Renderer->BeginScene();

	if(!m_hMesh.IsNull())
	{
		g_Renderer->SetTransformation(TRANS_VIEWPROJ,m_pCamera->GetViewProjMatrix());
		g_MeshMgr->DrawMesh(m_hMesh,Core::Matrix4::Identity,0,Core::HMaterial(),true);
	}

	g_Renderer->EndScene();
}

void CStaticMeshView::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_bDragging=true;
	m_iLastX=point.x;
	m_iLastY=point.y;

	CStatic::OnLButtonDown(nFlags,point);
}

void CStaticMeshView::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_bDragging=false;
	CStatic::OnLButtonUp(nFlags,point);
}

void CStaticMeshView::OnMouseMove(UINT nFlags, CPoint point)
{
	if(m_bDragging)
	{
		int dx=point.x - m_iLastX;
		int dy=point.y - m_iLastY;

		Core::Vector3 vFromTarget(m_pCamera->GetPosition() - m_pCamera->GetTarget());
		const Core::Quaternion qX(m_pCamera->GetXAxis(),(float)(dy)*0.02f);
		const Core::Quaternion qY(m_pCamera->GetYAxis(),(float)(dx)*0.02f);
		Core::Quaternion qRot(qY*qX);
		vFromTarget=qRot * vFromTarget;
			
		const float fdx=vFromTarget.y==0.0f ? 0.0f : fabs(vFromTarget.x / vFromTarget.y);
		const float fdz=vFromTarget.y==0.0f ? 0.0f : fabs(vFromTarget.z / vFromTarget.y);

		if(fdx > 0.02f && fdz > 0.02f)
		{
			m_pCamera->SetPosition(m_pCamera->GetTarget() + vFromTarget);
			Draw();

			m_iLastX=point.x;
			m_iLastY=point.y;
		}
	}

	CStatic::OnMouseMove(nFlags,point);
}

BOOL CStaticMeshView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	return CStatic::OnMouseWheel(nFlags,zDelta,pt);
}

void CStaticMeshView::OnPaint()
{
	CStatic::OnPaint();
	Draw();
}

void CStaticMeshView::SetMesh(Core::HMesh hMesh)
{
	m_hMesh=hMesh;
	if(!m_hMesh.IsNull())
	{
		IMesh *pMesh=g_MeshMgr->GetMesh(m_hMesh);
		if(pMesh)
		{
			const Core::Vector3 vSize(pMesh->GetBBox().m_vMax - pMesh->GetBBox().m_vMin);
			const Core::Vector3 vCenter((pMesh->GetBBox().m_vMin + pMesh->GetBBox().m_vMax) * 0.5f);
			const float fMaxSize=MAX(MAX(fabsf(vSize.x),fabsf(vSize.y)),fabsf(vSize.z));

			m_pCamera->SetTarget(vCenter);
			m_pCamera->SetPosition(Core::Vector3(0.0f,vCenter.y,-fMaxSize*1.5f));

			Draw();
		}
	}
}

