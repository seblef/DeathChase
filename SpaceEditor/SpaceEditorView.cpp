// SpaceEditorView.cpp : implémentation de la classe CSpaceEditorView
//

#include "stdafx.h"

#include "../DeathEngine/Export/IFactory.h"
#include "../DeathEngine/Export/IRenderer.h"
#include "../DeathEngine/Export/IMaterialMgr.h"

#include "Tools/ToolManager.h"
#include "Tools/ObjectTools.h"
#include "Selector.h"
#include "Axes.h"

#include "SpaceEditor.h"

#include "SpaceEditorDoc.h"
#include "SpaceEditorView.h"
#include "ShipPropDlg.h"

#include "../DeathEngine/Export/IMeshMgr.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CSpaceEditorView

IMPLEMENT_DYNCREATE(CSpaceEditorView, CView)

BEGIN_MESSAGE_MAP(CSpaceEditorView, CView)
	ON_COMMAND(ID_OUTILS_DEPLACER, &CSpaceEditorView::OnOutilsDeplacer)
	ON_COMMAND(ID_OUTILS_ECHELLE, &CSpaceEditorView::OnOutilsEchelle)
	ON_COMMAND(ID_OUTILS_SELECTION, &CSpaceEditorView::OnOutilsSelection)
	ON_COMMAND(ID_OUTILS_TOURNER, &CSpaceEditorView::OnOutilsTourner)
	ON_COMMAND(ID_OUTILS_VERROUILLERSURX, &CSpaceEditorView::OnOutilsVerrouillersurx)
	ON_COMMAND(ID_OUTILS_VERROUILLERSURY, &CSpaceEditorView::OnOutilsVerrouillersury)
	ON_COMMAND(ID_OUTILS_VERROUILLERSURZ, &CSpaceEditorView::OnOutilsVerrouillersurz)
	ON_COMMAND(ID_VUE_ARRIERE, &CSpaceEditorView::OnVueArriere)
	ON_COMMAND(ID_VUE_DEPLACER, &CSpaceEditorView::OnVueDeplacer)
	ON_COMMAND(ID_VUE_DESSOUS, &CSpaceEditorView::OnVueDessous)
	ON_COMMAND(ID_VUE_DESSUS, &CSpaceEditorView::OnVueDessus)
	ON_COMMAND(ID_VUE_DROITE, &CSpaceEditorView::OnVueDroite)
	ON_COMMAND(ID_VUE_FACE, &CSpaceEditorView::OnVueFace)
	ON_COMMAND(ID_VUE_GAUCHE, &CSpaceEditorView::OnVueGauche)
	ON_COMMAND(ID_VUE_PERSPECTIVE, &CSpaceEditorView::OnVuePerspective)
	ON_COMMAND(ID_VUE_TOURNER, &CSpaceEditorView::OnVueTourner)
	ON_COMMAND(ID_VUE_PROFONDEUR, &CSpaceEditorView::OnVueProfondeur)
	ON_COMMAND(ID_VUE_ZOOM, &CSpaceEditorView::OnVueZoom)
	ON_WM_DESTROY()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_PAINT()
	ON_WM_RBUTTONUP()
	ON_WM_SIZE()
	ON_WM_SETCURSOR()
	ON_COMMAND(ID_CREATE_MAINFIRE, &CSpaceEditorView::OnCreateMainfire)
	ON_COMMAND(ID_CREATE_SECFIRE, &CSpaceEditorView::OnCreateSecfire)
	ON_COMMAND(ID_CREATE_BACKFIRE, &CSpaceEditorView::OnCreateBackfire)
	ON_COMMAND(ID_CREATE_DEFENSE, &CSpaceEditorView::OnCreateDefense)
	ON_COMMAND(ID_CREATE_BOMBDROP, &CSpaceEditorView::OnCreateBombdrop)
END_MESSAGE_MAP()


const float fNoLightAmbientColor[4]={1.0f,1.0f,1.0f,1.0f};

// construction ou destruction de CSpaceEditorView

CSpaceEditorView::CSpaceEditorView() : m_bViewCreated(false), m_CurrentView(CAM_PERSPECTIVE),
	m_bDragging(false),m_vOldPos(-1,-1)
{
	for(int i=0;i<CAM_COUNT;i++)
		m_pCameras[i]=0;
}

CSpaceEditorView::~CSpaceEditorView()
{
}

BOOL CSpaceEditorView::PreCreateWindow(CREATESTRUCT& cs)
{
	for(int i=0;i<CAM_COUNT;i++)
	{
		m_pCameras[i]=(ICamera *)g_Factory->CreateObject(EOBJ_CAMERA);
		m_pCameras[i]->Initialize((CameraType)i,1.0f,10000.0f,400.0f,300.0f,90,2.0f);
	}

	return CView::PreCreateWindow(cs);
}

// dessin de CSpaceEditorView

void CSpaceEditorView::OnDraw(CDC* /*pDC*/)
{
	g_Renderer->SetView(m_hWnd);
	g_Renderer->BeginScene();

	g_Renderer->SetVertexShaderConstant(COLOR_AMBIENT_REG,fNoLightAmbientColor);

	Draw();

	g_ToolMgr.Draw();
	g_Selector.DrawSelected();
	g_Renderer->EndScene();
}


// diagnostics pour CSpaceEditorView

#ifdef _DEBUG
void CSpaceEditorView::AssertValid() const
{
	CView::AssertValid();
}

void CSpaceEditorView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CSpaceEditorDoc* CSpaceEditorView::GetDocument() const // la version non déboguée est en ligne
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSpaceEditorDoc)));
	return (CSpaceEditorDoc*)m_pDocument;
}
#endif //_DEBUG


// gestionnaires de messages pour CSpaceEditorView

void CSpaceEditorView::OnOutilsDeplacer()
{
	g_ToolMgr.SetTool(TOOL_TRANSLATE);
	UncheckToolMenu(0,ID_OUTILS_DEPLACER);
}

void CSpaceEditorView::OnOutilsEchelle()
{
	g_ToolMgr.SetTool(TOOL_SCALE);
	UncheckToolMenu(0,ID_OUTILS_ECHELLE);
}

void CSpaceEditorView::OnOutilsSelection()
{
	g_ToolMgr.SetTool(TOOL_SELECT);
	UncheckToolMenu(0,ID_OUTILS_SELECTION);
}

void CSpaceEditorView::OnOutilsTourner()
{
	g_ToolMgr.SetTool(TOOL_ROTATE);
	UncheckToolMenu(0,ID_OUTILS_TOURNER);
}

void CSpaceEditorView::OnOutilsVerrouillersurx()
{
	// TODO : ajoutez ici le code de votre gestionnaire de commande
}

void CSpaceEditorView::OnOutilsVerrouillersury()
{
	// TODO : ajoutez ici le code de votre gestionnaire de commande
}

void CSpaceEditorView::OnOutilsVerrouillersurz()
{
	// TODO : ajoutez ici le code de votre gestionnaire de commande
}

void CSpaceEditorView::OnVueArriere()
{
	m_CurrentView=CAM_BACK;
	OnDraw(0);

	UncheckViewMenu(0,ID_VUE_ARRIERE);
}

void CSpaceEditorView::OnVueDeplacer()
{
	g_ToolMgr.SetTool(TOOL_CAMMOVE);
	UncheckToolMenu(0,ID_VUE_DEPLACER);
}

void CSpaceEditorView::OnVueDessous()
{
	m_CurrentView=CAM_BOTTOM;
	OnDraw(0);

	UncheckViewMenu(0,ID_VUE_DESSOUS);
}

void CSpaceEditorView::OnVueDessus()
{
	m_CurrentView=CAM_TOP;
	OnDraw(0);

	UncheckViewMenu(0,ID_VUE_DESSUS);
}

void CSpaceEditorView::OnVueDroite()
{
	m_CurrentView=CAM_RIGHT;
	OnDraw(0);

	UncheckViewMenu(0,ID_VUE_DROITE);
}

void CSpaceEditorView::OnVueFace()
{
	m_CurrentView=CAM_FRONT;
	OnDraw(0);

	UncheckViewMenu(0,ID_VUE_FACE);
}

void CSpaceEditorView::OnVueGauche()
{
	m_CurrentView=CAM_LEFT;
	OnDraw(0);

	UncheckViewMenu(0,ID_VUE_GAUCHE);
}

void CSpaceEditorView::OnVuePerspective()
{
	m_CurrentView=CAM_PERSPECTIVE;
	OnDraw(0);

	UncheckViewMenu(0,ID_VUE_PERSPECTIVE);
}

void CSpaceEditorView::OnVueTourner()
{
	g_ToolMgr.SetTool(TOOL_CAMROTATE);
	UncheckToolMenu(0,ID_VUE_TOURNER);
}

void CSpaceEditorView::OnVueProfondeur()
{
	g_ToolMgr.SetTool(TOOL_CAMZMOVE);
	UncheckToolMenu(0,ID_VUE_PROFONDEUR);
}

void CSpaceEditorView::OnVueZoom()
{
	g_ToolMgr.SetTool(TOOL_CAMZOOM);
	UncheckToolMenu(0,ID_VUE_ZOOM);
}

void CSpaceEditorView::OnDestroy()
{
	g_Renderer->RemoveView(m_hWnd);

	for(int i=0;i<CAM_COUNT;i++)
	{
		g_Factory->ReleaseObject(m_pCameras[i]);
		m_pCameras[i]=0;
	}

	CView::OnDestroy();
}

void CSpaceEditorView::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_bDragging=true;
	g_ToolMgr.Start(m_pCameras[m_CurrentView], point.x, point.y);
	m_vOldPos.x=point.x;
	m_vOldPos.y=point.y;

	OnDraw(0);

	CView::OnLButtonDown(nFlags, point);
}

void CSpaceEditorView::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_bDragging=false;
	g_ToolMgr.End(point.x, point.y);

	OnDraw(0);

	CView::OnLButtonUp(nFlags, point);
}

void CSpaceEditorView::OnMouseMove(UINT nFlags, CPoint point)
{
	if(m_bDragging)
	{
		if(m_vOldPos.x==-1 && m_vOldPos.y==-1)
		{
			m_vOldPos.x=point.x;
			m_vOldPos.y=point.y;
			CView::OnMouseMove(nFlags, point);
			return;
		}

		int dx=point.x - m_vOldPos.x;
		int dy=point.y - m_vOldPos.y;

		if(dx==0 && dy==0)
		{
			CView::OnMouseMove(nFlags,point);
			return;
		}

		int iRes=g_ToolMgr.MouseMove(dx,dy);
		if(iRes!=0)
		{
			if(iRes & TR_REDRAWMAINVIEW)
				OnDraw(0);
		}

		m_vOldPos.x=point.x;
		m_vOldPos.y=point.y;
	}

	CView::OnMouseMove(nFlags, point);
}

void CSpaceEditorView::OnPaint()
{
	CPaintDC dc(this);

	OnDraw(0);
}

void CSpaceEditorView::OnRButtonUp(UINT nFlags, CPoint point)
{
	if(g_ToolMgr.GetCurrentToolType()==TOOL_SELECT)
	{
		CMenu menu,*context;

//		if(g_Selector.GetSelected()==0)
//		{
			if(!menu.LoadMenu(IDR_VIEWCONTEXT)) return;

			context=menu.GetSubMenu(0);

			UncheckToolMenu(context,ID_OUTILS_SELECTION);
			DWORD id;
			switch(m_CurrentView)
			{
				case CAM_PERSPECTIVE: id=ID_VUE_PERSPECTIVE; break;
				case CAM_FRONT: id=ID_VUE_FACE; break;
				case CAM_BACK: id=ID_VUE_ARRIERE; break;
				case CAM_TOP: id=ID_VUE_DESSUS; break;
				case CAM_BOTTOM: id=ID_VUE_DESSOUS; break;
				case CAM_LEFT: id=ID_VUE_GAUCHE; break;
				case CAM_RIGHT: id=ID_VUE_DROITE; break;
				default: id=ID_VUE_PERSPECTIVE; break;
			}
			UncheckViewMenu(context,id);
//		}
//		else
//		{
//			if(!menu.LoadMenu(IDR_OBJCONTEXT)) return;
//			context=menu.GetSubMenu(0);
//		}

		CPoint screenp(point);
		ClientToScreen(&screenp);
		context->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON,
									screenp.x, screenp.y, this);
	}
	else
	{
		g_ToolMgr.SetTool(TOOL_SELECT);
		SetCursor(g_ToolMgr.GetCursor());
	}

	CView::OnRButtonUp(nFlags, point);
}

void CSpaceEditorView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType,cx,cy);
	static int iCount=0;

	iCount++;
	if(iCount<3) return;

	if(nType!=SIZE_MINIMIZED)
	{
		if(!m_bViewCreated)
		{
			RECT rect;
			GetClientRect(&rect);
			ClientToScreen(&rect);

			g_Renderer->AddView(m_hWnd,rect.right-rect.left,rect.bottom-rect.top);
			m_bViewCreated=true;

			g_Engine->EnableLight(false);
			g_Engine->EnableShadows(false);
		}
		else
			g_Renderer->ResizeView(m_hWnd,cx,cy);

		for(int i=0;i<CAM_COUNT;i++)
			m_pCameras[i]->SetCenter(((float)cx)*0.5f,((float)cy)*0.5f);

		UncheckViewMenu(0,ID_VUE_PERSPECTIVE);
		UncheckToolMenu(0,ID_OUTILS_SELECTION);
	}
}

void CSpaceEditorView::OnUpdate(CView *pSender, LPARAM lHint, CObject *pHint)
{
	if(lHint==UPDATE_SHIPCREATED)
	{
		CSpaceEditorDoc *pDoc=GetDocument();

		if(pDoc->m_Objects.size()==0) return;

		Core::BBox bScene;
		bScene.Init();

		std::vector<SObject*>::iterator i;
		for(i=pDoc->m_Objects.begin();i!=pDoc->m_Objects.end();i++)
		{
			(*i)->GetTransformation().RebuildMatrix();
			bScene.Adjust(&(*i)->GetBSphere());
		}

		Core::Vector3 vCenter(bScene.GetCenter());
		Core::Vector3 vSize(bScene.GetSize());
		const float fMaxSize=MAX(MAX(fabsf(vSize.x),fabsf(vSize.y)),fabsf(vSize.z));

		CRect rect;
		GetClientRect(&rect);
		const float fZMax=fMaxSize*5.0f;
		m_pCameras[CAM_PERSPECTIVE]->SetTarget(vCenter);
		m_pCameras[CAM_PERSPECTIVE]->SetPosition(Core::Vector3(vCenter.x,vCenter.y,vCenter.z - fMaxSize*1.5f));

		const float fZoom=(float)rect.Width() / (fMaxSize==0.0f ? 1.0f : fMaxSize);

		m_pCameras[CAM_FRONT]->SetZoom(fZoom);
		m_pCameras[CAM_FRONT]->SetPosition(Core::Vector3(vCenter.x,vCenter.y,-256.0f));

		m_pCameras[CAM_BACK]->SetZoom(fZoom);
		m_pCameras[CAM_BACK]->SetPosition(Core::Vector3(vCenter.x,vCenter.y, 256.0f));

		m_pCameras[CAM_LEFT]->SetZoom(fZoom);
		m_pCameras[CAM_LEFT]->SetPosition(Core::Vector3(-256.0f,vCenter.y,vCenter.z));

		m_pCameras[CAM_RIGHT]->SetZoom(fZoom);
		m_pCameras[CAM_RIGHT]->SetPosition(Core::Vector3(256.0f,vCenter.y,vCenter.z));

		m_pCameras[CAM_TOP]->SetZoom(fZoom);
		m_pCameras[CAM_TOP]->SetPosition(Core::Vector3(vCenter.x,256.0f,vCenter.z));

		m_pCameras[CAM_BOTTOM]->SetZoom(fZoom);
		m_pCameras[CAM_BOTTOM]->SetPosition(Core::Vector3(vCenter.x,-256.0f,vCenter.z));
	}
	
	CView::OnUpdate(pSender,lHint,pHint);
}

void CSpaceEditorView::UncheckToolMenu(CMenu *pMenu,DWORD dwID)
{
	if(!pMenu)
		pMenu=AfxGetMainWnd()->GetMenu();

	if(!pMenu) return;

	pMenu->CheckMenuItem(ID_OUTILS_DEPLACER,MF_UNCHECKED);
	pMenu->CheckMenuItem(ID_OUTILS_ECHELLE,MF_UNCHECKED);
	pMenu->CheckMenuItem(ID_OUTILS_SELECTION,MF_UNCHECKED);
	pMenu->CheckMenuItem(ID_OUTILS_TOURNER,MF_UNCHECKED);
	pMenu->CheckMenuItem(ID_VUE_DEPLACER,MF_UNCHECKED);
	pMenu->CheckMenuItem(ID_VUE_TOURNER,MF_UNCHECKED);
	pMenu->CheckMenuItem(ID_VUE_PROFONDEUR,MF_UNCHECKED);
	pMenu->CheckMenuItem(ID_VUE_ZOOM,MF_UNCHECKED);

	pMenu->CheckMenuItem(dwID,MF_CHECKED);
}

void CSpaceEditorView::UncheckViewMenu(CMenu *pMenu,DWORD dwID)
{
	if(!pMenu)
		pMenu=AfxGetMainWnd()->GetMenu();
	if(!pMenu) return;

	pMenu->CheckMenuItem(ID_VUE_ARRIERE,MF_UNCHECKED);
	pMenu->CheckMenuItem(ID_VUE_DESSOUS,MF_UNCHECKED);
	pMenu->CheckMenuItem(ID_VUE_DESSUS,MF_UNCHECKED);
	pMenu->CheckMenuItem(ID_VUE_DROITE,MF_UNCHECKED);
	pMenu->CheckMenuItem(ID_VUE_FACE,MF_UNCHECKED);
	pMenu->CheckMenuItem(ID_VUE_GAUCHE,MF_UNCHECKED);
	pMenu->CheckMenuItem(ID_VUE_PERSPECTIVE,MF_UNCHECKED);

	pMenu->CheckMenuItem(dwID,MF_CHECKED);
}

BOOL CSpaceEditorView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	SetCursor(g_ToolMgr.GetCursor());
	return TRUE;
}

void CSpaceEditorView::Draw()
{
	g_Renderer->SetTransformation(TRANS_VIEWPROJ,m_pCameras[m_CurrentView]->GetViewProjMatrix());
	g_MaterialMgr->SetRenderingPass(RENDERINGPASS_AMBIENT);
	g_MeshMgr->SetCameraPosition(m_pCameras[m_CurrentView]->GetPosition());

	CSpaceEditorDoc *pDoc=GetDocument();
	const IViewFrustrum *pFrustrum=m_pCameras[m_CurrentView]->GetFrustrum();

	std::vector<SObject*>::iterator i;
	for(i=pDoc->m_Objects.begin();i!=pDoc->m_Objects.end();i++)
	{
		if(pFrustrum->IsSphereInside((*i)->GetBSphere()))
			(*i)->Draw();
	}

	g_MeshMgr->Execute();

	for(i=pDoc->m_Objects.begin();i!=pDoc->m_Objects.end();i++)
	{
		if(pFrustrum->IsSphereInside((*i)->GetBSphere()))
			Axes::Draw((*i)->GetTransformation().GetMatrix());
	}

	Axes::Draw(Core::Matrix4::Identity);
}

void CSpaceEditorView::OnCreateMainfire()
{
	SObject *pObj=new SObject(SOBJ_MAINFIRE);
	pObj->ConstructMesh();
	g_ToolMgr.SetTool(TOOL_TRANSLATE);
	((ToolTranslate *)g_ToolMgr.GetCurrentTool())->ForceObject(pObj);

	GetDocument()->m_Objects.push_back(pObj);
	g_Selector.RegisterObject(pObj);
}

void CSpaceEditorView::OnCreateSecfire()
{
	// TODO : ajoutez ici le code de votre gestionnaire de commande
}

void CSpaceEditorView::OnCreateBackfire()
{
	// TODO : ajoutez ici le code de votre gestionnaire de commande
}

void CSpaceEditorView::OnCreateDefense()
{
	// TODO : ajoutez ici le code de votre gestionnaire de commande
}

void CSpaceEditorView::OnCreateBombdrop()
{
	// TODO : ajoutez ici le code de votre gestionnaire de commande
}
