// MainFrm.h : interface de la classe CMainFrame
//

#include "MFC/CSplitterWndEx.h"

#pragma once

class CMainFrame : public CFrameWnd
{
	
protected: // cr�ation � partir de la s�rialisation uniquement
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributs
public:

// Op�rations
public:

// Substitutions
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);

// Impl�mentation
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

// Fonctions g�n�r�es de la table des messages
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDevConsole();
	afx_msg void OnDevMemoire();
};


