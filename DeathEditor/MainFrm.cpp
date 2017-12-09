// MainFrm.cpp : implémentation de la classe CMainFrame
//

#include "stdafx.h"
#include "DeathEditor.h"

#include "MainFrm.h"
#include "Control.h"
#include "DeathEditorDoc.h"
#include "DeathEditorView.h"

#ifdef DEDEV
#include "Console.h"
#include "Memory.h"
#endif

#include "Interfaces.h"
#include "../DeathEngine/Export/IRenderer.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include "Tools/ToolManager.h"

#ifdef DEDEV
CMemory g_Memory;
CConsole g_Console;

void OutputConsole(const char *szMsg)
{
	if(g_Console.IsCreated())
	{
		const int ln=strlen(szMsg);

		static std::string strBuffer;
		strBuffer.clear();

		int i;
		for(i=0;i<ln;i++)
		{
			if(szMsg[i]=='\n')
			{
				g_Console.AddLine(strBuffer.c_str());
				strBuffer.clear();
			}
			else
				strBuffer+=szMsg[i];
		}
	}
}

#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(ID_DEV_CONSOLE, &CMainFrame::OnDevConsole)
	ON_COMMAND(ID_DEV_MEMOIRE, &CMainFrame::OnDevMemoire)
END_MESSAGE_MAP()

// construction ou destruction de CMainFrame

CMainFrame::CMainFrame() : m_bSplitterInited(false)
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

#ifdef DEDEV

	g_Console.Create(IDD_CONSOLE,this);
//	g_Console.ShowWindow(SW_SHOW);
	g_Console.ShowWindow(SW_HIDE);
	g_Output2->SetOutputRedirect(&OutputConsole);

	g_Memory.Create(IDD_MEMORY,this);
	g_Memory.ShowWindow(SW_HIDE);

#endif

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

	new ToolMgr;

	return 0;
}

BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	CRect cr;
	GetClientRect(&cr);

	if(!m_Splitter.CreateStatic(this,1,2))
	{
		MessageBox("Erreur lors de la création du panneau de controle","Erreur",MB_OK | MB_ICONERROR);
		return FALSE;
	}

	if(!m_Splitter.CreateView(0,0,RUNTIME_CLASS(CDeathEditorView),
		CSize(cr.Width() - 280,cr.Height()),pContext))
	{
		MessageBox("Erreur lors de la création de la vue principale","Erreur",MB_OK | MB_ICONERROR);
		return FALSE;
	}

	if(!m_Splitter.CreateView(0,1,RUNTIME_CLASS(CControl),
		CSize(280,cr.Height()),pContext))
	{
		MessageBox("Erreur lors de la création de la vue de contrôle","Erreur",MB_OK | MB_ICONERROR);
		return FALSE;
	}

	m_Splitter.SetThinFrame();
	m_bSplitterInited=true;

#ifndef DEDEV
	CMenu *pMenu=GetMenu();
	if(pMenu)
		pMenu->RemoveMenu(pMenu->GetMenuItemCount()-1,0);
#endif

	return TRUE;
}

void CMainFrame::OnSize(UINT nType, int cx, int cy) 
{
	CFrameWnd::OnSize(nType, cx, cy);
	
	if((nType!=SIZE_MINIMIZED) && m_bSplitterInited)
	{
		m_Splitter.SetColumnInfo(1,280,0);
		m_Splitter.SetColumnInfo(0,cx - 280,0);
		m_Splitter.SetRowInfo(0,cy,0);

		m_Splitter.RecalcLayout();
	}
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO : changez ici la classe ou les styles Window en modifiant
	//  CREATESTRUCT cs

	return TRUE;
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




void CMainFrame::OnDevConsole()
{
#ifdef DEDEV
	g_Console.ShowWindow(g_Console.IsWindowVisible() ? SW_HIDE : SW_SHOW);

	CMenu *pMenu=GetMenu();
	if(pMenu)
		pMenu->CheckMenuItem(ID_DEV_CONSOLE,g_Console.IsWindowVisible() ? MF_CHECKED : MF_UNCHECKED);

#endif
}

void CMainFrame::OnDevMemoire()
{
#ifdef DEDEV
	g_Memory.ShowWindow(g_Memory.IsWindowVisible() ? SW_HIDE : SW_SHOW);

	CMenu *pMenu=GetMenu();
	if(pMenu)
		pMenu->CheckMenuItem(ID_DEV_CONSOLE,g_Memory.IsWindowVisible() ? MF_CHECKED : MF_UNCHECKED);

#endif
}

