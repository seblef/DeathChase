// DeathEditorView.cpp : implémentation de la classe CDeathEditorView
//

#include "stdafx.h"
#include "DeathEditor.h"

#include "DeathEditorDoc.h"
#include "DeathEditorView.h"

#include "MapObjectPropDlg.h"
#include "MeshObjPropDlg.h"

#include "Tools/ToolManager.h"
#include "Tools/ObjectTools.h"

#include "Data/Selector.h"

#include "../DeathEngine/Export/IMeshMgr.h"
#include "../DeathEngine/Export/IFactory.h"
#include "../DeathEngine/Export/IRenderer.h"
#include "../DeathEngine/Export/IMap.h"
#include "../DeathEngine/Export/IHeightmap.h"
#include "../DeathEngine/Game/IMapObject.h"
#include "../DeathEngine/Export/IMapObjectFactory.h"
#include "../DeathEngine/Export/IObjectMap.h"
#include "../DeathEngine/Export/ISun.h"
#include "../DeathEngine/Export/IMeshObject.h"
#include "../DeathEngine/Export/IMesh.h"

#include "FileSelectDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CDeathEditorView

IMPLEMENT_DYNCREATE(CDeathEditorView, CView)

BEGIN_MESSAGE_MAP(CDeathEditorView, CView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_COMMAND(ID_OUTILS_ALIGNER, &CDeathEditorView::OnOutilsAligner)
	ON_COMMAND(ID_OUTILS_DEPLACER, &CDeathEditorView::OnOutilsDeplacer)
	ON_COMMAND(ID_OUTILS_ECHELLE, &CDeathEditorView::OnOutilsEchelle)
	ON_COMMAND(ID_OUTILS_LIER, &CDeathEditorView::OnOutilsLier)
	ON_COMMAND(ID_OUTILS_SELECTION, &CDeathEditorView::OnOutilsSelection)
	ON_COMMAND(ID_OUTILS_TOURNER, &CDeathEditorView::OnOutilsTourner)
	ON_COMMAND(ID_OUTILS_VERROUILLERSURX, &CDeathEditorView::OnOutilsVerrouillersurx)
	ON_COMMAND(ID_OUTILS_VERROUILLERSURY, &CDeathEditorView::OnOutilsVerrouillersury)
	ON_COMMAND(ID_OUTILS_VERROUILLERSURZ, &CDeathEditorView::OnOutilsVerrouillersurz)
	ON_COMMAND(ID_VUE_ARRIERE, &CDeathEditorView::OnVueArriere)
	ON_COMMAND(ID_VUE_CENTRER, &CDeathEditorView::OnVueCentrer)
	ON_COMMAND(ID_VUE_DEPLACER, &CDeathEditorView::OnVueDeplacer)
	ON_COMMAND(ID_VUE_DESSOUS, &CDeathEditorView::OnVueDessous)
	ON_COMMAND(ID_VUE_DESSUS, &CDeathEditorView::OnVueDessus)
	ON_COMMAND(ID_VUE_DROITE, &CDeathEditorView::OnVueDroite)
	ON_COMMAND(ID_VUE_FACE, &CDeathEditorView::OnVueFace)
	ON_COMMAND(ID_VUE_GAUCHE, &CDeathEditorView::OnVueGauche)
	ON_COMMAND(ID_VUE_PERSPECTIVE, &CDeathEditorView::OnVuePerspective)
	ON_COMMAND(ID_VUE_PROFONDEUR, &CDeathEditorView::OnVueProfondeur)
	ON_COMMAND(ID_VUE_TOURNER, &CDeathEditorView::OnVueTourner)
	ON_COMMAND(ID_VUE_ZOOM, &CDeathEditorView::OnVueZoom)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_SETCURSOR()
	ON_WM_RBUTTONUP()
	ON_WM_KILLFOCUS()
	ON_WM_CAPTURECHANGED()
	ON_COMMAND(ID_OPTIONS_ECLAIRAGE, &CDeathEditorView::OnOptionsEclairage)
	ON_COMMAND(ID_OPTIONS_OMBRES, &CDeathEditorView::OnOptionsOmbres)
	ON_COMMAND(ID_CREER_MAPOBJECT, &CDeathEditorView::OnCreerMapobject)
	ON_COMMAND(ID_CREER_OBJET, &CDeathEditorView::OnCreerObjet)
	ON_COMMAND(ID_OBJ_COPY, &CDeathEditorView::OnObjCopy)
	ON_COMMAND(ID_OBJ_PASTE, &CDeathEditorView::OnObjPaste)
	ON_COMMAND(ID_OBJ_PROP, &CDeathEditorView::OnObjProp)
	ON_COMMAND(ID_OBJ_REMOVE, &CDeathEditorView::OnObjRemove)
END_MESSAGE_MAP()

// construction ou destruction de CDeathEditorView

CDeathEditorView::CDeathEditorView() : m_bViewCreated(false), m_CurrentView(CAM_PERSPECTIVE),
	m_bDragging(false),m_vOldPos(-1,-1), m_bLightEnable(false), m_bShadowsEnable(false), m_bCreated(false)
{
	for(int i=0;i<CAM_COUNT;i++)
		m_pCameras[i]=0;

}

CDeathEditorView::~CDeathEditorView()
{
}

int CDeathEditorView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	for(int i=0;i<CAM_COUNT;i++)
	{
		m_pCameras[i]=(ICamera *)g_Factory->CreateObject(EOBJ_CAMERA);
		m_pCameras[i]->Initialize((CameraType)i,1.0f,10000.0f,400.0f,300.0f,90,2.0f);
	}

	return 0;
}

void CDeathEditorView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType,cx,cy);
	static bool bFirstSize=true;

	if(bFirstSize)
	{
		bFirstSize=false;
		return;
	}

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

void CDeathEditorView::OnDestroy()
{
	g_Renderer->RemoveView(m_hWnd);

	for(int i=0;i<CAM_COUNT;i++)
	{
		g_Factory->ReleaseObject(m_pCameras[i]);
		m_pCameras[i]=0;
	}

	CView::OnDestroy();
}

BOOL CDeathEditorView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO : changez ici la classe ou les styles Window en modifiant
	//  CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// dessin de CDeathEditorView

void CDeathEditorView::OnDraw(CDC* /*pDC*/)
{
	g_Renderer->SetView(m_hWnd);
	g_Renderer->BeginScene();

	switch(GetDocument()->GetCurrentEditor())
	{
		case EDITOR_MAP:
			DrawMap();
			break;

		default:
			break;
	}

	g_ToolMgr.Draw();
	g_Selector.DrawSelected();
	g_Renderer->EndScene();
}

void CDeathEditorView::OnUpdate(CView *pSender, LPARAM lHint, CObject *pHint)
{
	if(lHint==UPDATE_OPENMAP || lHint==UPDATE_NEWMAP)
	{
		const float fMidMap=(float)g_Map->GetHeightmap()->GetSize() / 2;
		const float fHeight=g_Map->GetHeightmap()->GetHeight((int)fMidMap,(int)fMidMap);
		m_pCameras[CAM_PERSPECTIVE]->SetTarget(Core::Vector3(fMidMap,fHeight,fMidMap));
		m_pCameras[CAM_PERSPECTIVE]->SetPosition(Core::Vector3(0.0f,256.0f,0.0f));

		m_pCameras[CAM_FRONT]->SetZoom(2.0f);
		m_pCameras[CAM_FRONT]->SetPosition(Core::Vector3(fMidMap,128.0f,0.0f));

		m_pCameras[CAM_BACK]->SetZoom(2.0f);
		m_pCameras[CAM_BACK]->SetPosition(Core::Vector3(fMidMap,128.0f,fMidMap*2.0f));

		m_pCameras[CAM_LEFT]->SetZoom(2.0f);
		m_pCameras[CAM_LEFT]->SetPosition(Core::Vector3(0.0f,128.0f,fMidMap));

		m_pCameras[CAM_RIGHT]->SetZoom(2.0f);
		m_pCameras[CAM_RIGHT]->SetPosition(Core::Vector3(fMidMap*2.0f,128.0f,fMidMap));

		m_pCameras[CAM_TOP]->SetZoom(2.0f);
		m_pCameras[CAM_TOP]->SetPosition(Core::Vector3(fMidMap,256.0f,fMidMap));

		m_pCameras[CAM_BOTTOM]->SetZoom(2.0f);
		m_pCameras[CAM_BOTTOM]->SetPosition(Core::Vector3(fMidMap,-1.0f,fMidMap));

		m_EditorSun.Load("Models\\EditorSun\\EditorSun.dgeo",0);
		g_Selector.AddObject(&m_EditorSun);
	}
	
	if(lHint==UPDATE_MAPCREATED || lHint==UPDATE_OPENMAP)
	{
		const float fMidMap=(float)g_Map->GetHeightmap()->GetSize() / 2;

		const Core::Vector3 vStart(fMidMap,0.0f,fMidMap);
		m_EditorSun.GetTransformation().SetPosition(vStart - g_Map->GetSun()->GetDirection() * fMidMap);
		m_EditorSun.GetTransformation().RebuildMatrix();

		GetDocument()->UpdateAllViews(0,UPDATE_NEWOBJECT,(CObject *)((void *)&m_EditorSun));
		
		m_bCreated=true;
	}
	else if(lHint==UPDATE_RESET)
		m_bCreated=false;

	CView::OnUpdate(pSender,lHint,pHint);
}


// diagnostics pour CDeathEditorView

#ifdef _DEBUG
void CDeathEditorView::AssertValid() const
{
	CView::AssertValid();
}

void CDeathEditorView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CDeathEditorDoc* CDeathEditorView::GetDocument() const // la version non déboguée est en ligne
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDeathEditorDoc)));
	return (CDeathEditorDoc*)m_pDocument;
}
#endif //_DEBUG


// gestionnaires de messages pour CDeathEditorView

void CDeathEditorView::OnOutilsAligner()
{
	g_ToolMgr.SetTool(TOOL_ALIGN);
	UncheckToolMenu(0,ID_OUTILS_ALIGNER);
}

void CDeathEditorView::OnOutilsDeplacer()
{
	g_ToolMgr.SetTool(TOOL_TRANSLATE);
	UncheckToolMenu(0,ID_OUTILS_DEPLACER);
}

void CDeathEditorView::OnOutilsEchelle()
{
	g_ToolMgr.SetTool(TOOL_SCALE);
	UncheckToolMenu(0,ID_OUTILS_ECHELLE);
}

void CDeathEditorView::OnOutilsLier()
{
	g_ToolMgr.SetTool(TOOL_LINK);
	UncheckToolMenu(0,ID_OUTILS_LIER);
}

void CDeathEditorView::OnOutilsSelection()
{
	g_ToolMgr.SetTool(TOOL_SELECT);
	UncheckToolMenu(0,ID_OUTILS_SELECTION);
}

void CDeathEditorView::OnOutilsTourner()
{
	g_ToolMgr.SetTool(TOOL_ROTATE);
	UncheckToolMenu(0,ID_OUTILS_TOURNER);
}

void CDeathEditorView::OnOutilsVerrouillersurx()
{
	// TODO : ajoutez ici le code de votre gestionnaire de commande
}

void CDeathEditorView::OnOutilsVerrouillersury()
{
	// TODO : ajoutez ici le code de votre gestionnaire de commande
}

void CDeathEditorView::OnOutilsVerrouillersurz()
{
	// TODO : ajoutez ici le code de votre gestionnaire de commande
}

void CDeathEditorView::OnVueArriere()
{
	m_CurrentView=CAM_BACK;
	OnDraw(0);

	UncheckViewMenu(0,ID_VUE_ARRIERE);
}

void CDeathEditorView::OnVueCentrer()
{
	// TODO : ajoutez ici le code de votre gestionnaire de commande
}

void CDeathEditorView::OnVueDeplacer()
{
	g_ToolMgr.SetTool(TOOL_CAMMOVE);
	UncheckToolMenu(0,ID_VUE_DEPLACER);
}

void CDeathEditorView::OnVueDessous()
{
	m_CurrentView=CAM_BOTTOM;
	OnDraw(0);

	UncheckViewMenu(0,ID_VUE_DESSOUS);
}

void CDeathEditorView::OnVueDessus()
{
	m_CurrentView=CAM_TOP;
	OnDraw(0);

	UncheckViewMenu(0,ID_VUE_DESSUS);
}

void CDeathEditorView::OnVueDroite()
{
	m_CurrentView=CAM_RIGHT;
	OnDraw(0);

	UncheckViewMenu(0,ID_VUE_DROITE);
}

void CDeathEditorView::OnVueFace()
{
	m_CurrentView=CAM_FRONT;
	OnDraw(0);

	UncheckViewMenu(0,ID_VUE_FACE);
}

void CDeathEditorView::OnVueGauche()
{
	m_CurrentView=CAM_LEFT;
	OnDraw(0);

	UncheckViewMenu(0,ID_VUE_GAUCHE);
}

void CDeathEditorView::OnVuePerspective()
{
	m_CurrentView=CAM_PERSPECTIVE;
	OnDraw(0);

	UncheckViewMenu(0,ID_VUE_PERSPECTIVE);
}

void CDeathEditorView::OnVueProfondeur()
{
	g_ToolMgr.SetTool(TOOL_CAMZMOVE);
	UncheckToolMenu(0,ID_VUE_PROFONDEUR);
}

void CDeathEditorView::OnVueTourner()
{
	g_ToolMgr.SetTool(TOOL_CAMROTATE);
	UncheckToolMenu(0,ID_VUE_TOURNER);
}

void CDeathEditorView::OnVueZoom()
{
	g_ToolMgr.SetTool(TOOL_CAMZOOM);
	UncheckToolMenu(0,ID_VUE_ZOOM);
}

void CDeathEditorView::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_bDragging=true;
	g_ToolMgr.Start(m_pCameras[m_CurrentView], point.x, point.y);
	m_vOldPos.x=point.x;
	m_vOldPos.y=point.y;

	OnDraw(0);

	CView::OnLButtonDown(nFlags, point);
}

void CDeathEditorView::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_bDragging=false;
	g_ToolMgr.End(point.x, point.y);

	OnDraw(0);

	CView::OnLButtonUp(nFlags, point);
}

void CDeathEditorView::OnMouseMove(UINT nFlags, CPoint point)
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
			if(iRes & TR_CREATEOBJECT)
				GetDocument()->UpdateAllViews(0,UPDATE_NEWOBJECT,(CObject *)((void *)g_Selector.GetSelected()));
		}

		m_vOldPos.x=point.x;
		m_vOldPos.y=point.y;
	}

	CView::OnMouseMove(nFlags, point);
}

void CDeathEditorView::UncheckToolMenu(CMenu *pMenu,DWORD dwID)
{
	if(!pMenu)
		pMenu=AfxGetMainWnd()->GetMenu();

	if(!pMenu) return;

	pMenu->CheckMenuItem(ID_OUTILS_ALIGNER,MF_UNCHECKED);
	pMenu->CheckMenuItem(ID_OUTILS_DEPLACER,MF_UNCHECKED);
	pMenu->CheckMenuItem(ID_OUTILS_ECHELLE,MF_UNCHECKED);
	pMenu->CheckMenuItem(ID_OUTILS_LIER,MF_UNCHECKED);
	pMenu->CheckMenuItem(ID_OUTILS_SELECTION,MF_UNCHECKED);
	pMenu->CheckMenuItem(ID_OUTILS_TOURNER,MF_UNCHECKED);
	pMenu->CheckMenuItem(ID_VUE_DEPLACER,MF_UNCHECKED);
	pMenu->CheckMenuItem(ID_VUE_TOURNER,MF_UNCHECKED);
	pMenu->CheckMenuItem(ID_VUE_PROFONDEUR,MF_UNCHECKED);
	pMenu->CheckMenuItem(ID_VUE_ZOOM,MF_UNCHECKED);

	pMenu->CheckMenuItem(dwID,MF_CHECKED);
}

void CDeathEditorView::UncheckViewMenu(CMenu *pMenu,DWORD dwID)
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

BOOL CDeathEditorView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	SetCursor(g_ToolMgr.GetCursor());
	return TRUE;
}

void CDeathEditorView::OnRButtonUp(UINT nFlags, CPoint point)
{
	if(g_ToolMgr.GetCurrentToolType()==TOOL_SELECT)
	{
		CMenu menu,*context;

		if(g_Selector.GetSelected()==0)
		{
			if(!menu.LoadMenu(IDR_VIEWCONTEXT)) return;

			context=menu.GetSubMenu(0);

			const UINT nCmd=m_bCreated ? (MF_BYPOSITION | MF_ENABLED) : (MF_BYPOSITION | MF_DISABLED);
	//		context->EnableMenuItem(1,nCmd);
	//		context->EnableMenuItem(2,nCmd);
			context->EnableMenuItem(4,nCmd);

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
		}
		else
		{
			if(!menu.LoadMenu(IDR_OBJCONTEXT)) return;
			context=menu.GetSubMenu(0);
		}

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

void CDeathEditorView::OnKillFocus(CWnd* pNewWnd)
{
	CView::OnKillFocus(pNewWnd);

	m_bDragging=false;
	g_ToolMgr.End(-10,-10);
}

void CDeathEditorView::OnCaptureChanged(CWnd *pWnd)
{
	m_bDragging=false;
	g_ToolMgr.End(-10,-10);

	CView::OnCaptureChanged(pWnd);
}

void CDeathEditorView::DrawMap()
{
	Core::Matrix4 mViewProj(m_pCameras[m_CurrentView]->GetViewProjMatrix());
	g_Renderer->SetTransformation(TRANS_VIEWPROJ,mViewProj);

	g_Map->Draw(m_pCameras[m_CurrentView]->GetFrustrum());
	m_EditorSun.Draw();
}

void CDeathEditorView::OnOptionsEclairage()
{
	m_bLightEnable=!m_bLightEnable;
	g_Engine->EnableLight(m_bLightEnable);

	GetDocument()->UpdateAllViews(0);
}

void CDeathEditorView::OnOptionsOmbres()
{
	m_bShadowsEnable=!m_bShadowsEnable;
	g_Engine->EnableShadows(m_bShadowsEnable);

	GetDocument()->UpdateAllViews(0);
}

void CDeathEditorView::OnCreerMapobject()
{
	CFileSelectDlg dlg;
	dlg.SetFileType(FILE_MESH);

	if(dlg.DoModal()==IDCANCEL) return;

	IMapObject *pObj=(IMapObject *)g_Map->GetObjectFactory()->CreateObject(OBJ_MAP);

	if(!pObj->Generate(dlg.m_strPathName + dlg.m_strFileName,0))
	{
		MessageBox("Erreur à la création de l'objet","Erreur",MB_OK | MB_ICONERROR);
		g_Map->GetObjectFactory()->ReleaseObject(pObj);
		return;
	}

	g_ToolMgr.SetTool(TOOL_TRANSLATE);
	((ToolTranslate *)g_ToolMgr.GetCurrentTool())->ForceObject(pObj);
}

void CDeathEditorView::OnCreerObjet()
{
	CFileSelectDlg dlg;
	dlg.SetFileType(FILE_MESH);

	if(dlg.DoModal()==IDCANCEL) return;

	Core::HMesh hMesh=g_MeshMgr->LoadMesh(dlg.m_strPathName + dlg.m_strFileName,0);

	if(hMesh.IsNull())
	{
		MessageBox("Erreur au chargement de l'objet.","Erreur",MB_OK | MB_ICONERROR);
		return;
	}

	IMeshObject *pObj=(IMeshObject *)g_Map->GetObjectFactory()->CreateObject(OBJ_MESH);
	pObj->SetMesh(hMesh);

	g_ToolMgr.SetTool(TOOL_TRANSLATE);
	((ToolTranslate *)g_ToolMgr.GetCurrentTool())->ForceObject(pObj);
}

void CDeathEditorView::OnObjCopy()
{
	// TODO : ajoutez ici le code de votre gestionnaire de commande
}

void CDeathEditorView::OnObjPaste()
{
	// TODO : ajoutez ici le code de votre gestionnaire de commande
}

void CDeathEditorView::OnObjProp()
{
	Game::IObject *pObj=g_Selector.GetSelected();
	if(!pObj) return;

	ObjectType type=pObj->GetObjectType();
	if(type==OBJ_MAP)
	{
		CString strName(pObj->GetName());

		CMapObjectPropDlg dlg(pObj);
		dlg.DoModal();

		if(!(strName==dlg.m_strName))
			GetDocument()->UpdateAllViews(0,UPDATE_MODIFYOBJECT,(CObject *)((void *)pObj));
	}
	else if(type==OBJ_MESH)
	{
		CString strName(pObj->GetName());

		CMeshObjPropDlg dlg((IMeshObject *)pObj);
		dlg.DoModal();

		if(!(strName==dlg.m_strName))
			GetDocument()->UpdateAllViews(0,UPDATE_MODIFYOBJECT,(CObject *)((void *)pObj));
	}
}

void CDeathEditorView::OnObjRemove()
{
	Game::IObject *pObj=g_Selector.GetSelected();
	if(!pObj) return;

	pObj->RemoveFromOctree();
	g_Selector.ClearSelection();

	GetDocument()->UpdateAllViews(0,UPDATE_REMOVEOBJECT,(CObject *)((void *)pObj));
	g_Map->GetObjectFactory()->ReleaseObject(pObj);
}
