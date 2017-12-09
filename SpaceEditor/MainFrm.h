// MainFrm.h : interface de la classe CMainFrame
//


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

// Implémentation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // membres incorporés de la barre de contrôle
	CToolBar    m_ToolBar;

// Fonctions générées de la table des messages
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	DECLARE_MESSAGE_MAP()
};


