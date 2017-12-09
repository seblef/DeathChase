// MainFrm.cpp : implémentation de la classe CMainFrame
//


#include "stdafx.h"

#include "../DeathEditor/Interfaces.h"
#include "../DeathEngine/Export/IRenderer.h"

#include "SpaceEditor.h"
#include "MainFrm.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_SIZE()
	ON_WM_CREATE()
END_MESSAGE_MAP()


// construction ou destruction de CMainFrame

CMainFrame::CMainFrame()
{
	// TODO : ajoutez ici le code d'une initialisation de membre
}

CMainFrame::~CMainFrame()
{
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	g_Renderer->Initialize(m_hWnd,theApp.m_hInstance,10,10);

	if(!m_ToolBar.CreateEx(this,TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP ) ||
	   !m_ToolBar.LoadToolBar(IDR_MAINTOOLBAR))
	{
		TRACE0("Impossible de créer la bare d'outil Vue\n");
		return 0;
	}

	m_ToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_TOP);
	DockControlBar(&m_ToolBar);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO : changez ici la classe ou les styles Window en modifiant
	//  CREATESTRUCT cs

	return TRUE;
}

void CMainFrame::OnSize(UINT nType, int cx, int cy) 
{
	CFrameWnd::OnSize(nType, cx, cy);
}

// diagnostics pour CMainFrame

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG


// gestionnaires de messages pour CMainFrame



