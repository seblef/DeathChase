// MainFrm.h : interface de la classe CMainFrame
//

#include "MFC/CSplitterWndEx.h"

#pragma once

class CMainFrame : public CFrameWnd
{
	
protected: // création à partir de la sérialisation uniquement
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributs
public:

// Opérations
public:

// Substitutions
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);

// Implémentation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

	CSplitterWndEx m_Splitter;
	bool m_bSplitterInited;
	CToolBar m_ToolBar;

// Fonctions générées de la table des messages
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDevConsole();
	afx_msg void OnDevMemoire();
};


