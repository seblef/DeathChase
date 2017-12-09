// DeathEditorView.h : interface de la classe CDeathEditorView
//

#include "Interfaces.h"
#include "../DeathEngine/Export/ICamera.h"
#include "Data/EditorSun.h"

#pragma once


class CDeathEditorView : public CView
{
protected: // création à partir de la sérialisation uniquement
	CDeathEditorView();
	DECLARE_DYNCREATE(CDeathEditorView)

// Attributs
public:
	CDeathEditorDoc* GetDocument() const;

// Opérations
public:

// Substitutions
public:
	virtual void OnDraw(CDC* pDC);  // substitué pour dessiner cette vue
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// Implémentation
public:
	virtual ~CDeathEditorView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

	void UncheckToolMenu(CMenu *pMenu,DWORD dwID);
	void UncheckViewMenu(CMenu *pMenu,DWORD dwID);

	bool m_bViewCreated;
	ICamera *m_pCameras[CAM_COUNT];
	CameraType m_CurrentView;
	bool m_bDragging;
	Core::Vector2i m_vOldPos;

	bool m_bLightEnable;
	bool m_bShadowsEnable;

	EditorSun m_EditorSun;
	bool m_bCreated;

	void DrawMap();


// Fonctions générées de la table des messages
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnOutilsAligner();
	afx_msg void OnOutilsDeplacer();
	afx_msg void OnOutilsEchelle();
	afx_msg void OnOutilsLier();
	afx_msg void OnOutilsSelection();
	afx_msg void OnOutilsTourner();
	afx_msg void OnOutilsVerrouillersurx();
	afx_msg void OnOutilsVerrouillersury();
	afx_msg void OnOutilsVerrouillersurz();
	afx_msg void OnVueArriere();
	afx_msg void OnVueCentrer();
	afx_msg void OnVueDeplacer();
	afx_msg void OnVueDessous();
	afx_msg void OnVueDessus();
	afx_msg void OnVueDroite();
	afx_msg void OnVueFace();
	afx_msg void OnVueGauche();
	afx_msg void OnVuePerspective();
	afx_msg void OnVueProfondeur();
	afx_msg void OnVueTourner();
	afx_msg void OnVueZoom();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnCaptureChanged(CWnd *pWnd);
	afx_msg void OnOptionsEclairage();
	afx_msg void OnOptionsOmbres();
	afx_msg void OnCreerMapobject();
	afx_msg void OnCreerObjet();
	afx_msg void OnObjCopy();
	afx_msg void OnObjPaste();
	afx_msg void OnObjProp();
	afx_msg void OnObjRemove();
};

#ifndef _DEBUG  // version de débogage dans DeathEditorView.cpp
inline CDeathEditorDoc* CDeathEditorView::GetDocument() const
   { return reinterpret_cast<CDeathEditorDoc*>(m_pDocument); }
#endif

