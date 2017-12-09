// SpaceEditorView.h : interface de la classe CSpaceEditorView
//

#include "../DeathEditor/Interfaces.h"
#include "../DeathEngine/Export/ICamera.h"

#pragma once


class CSpaceEditorView : public CView
{
protected: // création à partir de la sérialisation uniquement
	CSpaceEditorView();
	DECLARE_DYNCREATE(CSpaceEditorView)

// Attributs
public:
	CSpaceEditorDoc* GetDocument() const;

// Opérations
public:

// Substitutions
public:
	virtual void OnDraw(CDC* pDC);  // substitué pour dessiner cette vue
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

	void UncheckToolMenu(CMenu *pMenu,DWORD dwID);
	void UncheckViewMenu(CMenu *pMenu,DWORD dwID);

	void Draw();

	bool m_bViewCreated;
	ICamera *m_pCameras[CAM_COUNT];
	CameraType m_CurrentView;
	bool m_bDragging;
	Core::Vector2i m_vOldPos;

	// Implémentation
public:
	virtual ~CSpaceEditorView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Fonctions générées de la table des messages
protected:
	DECLARE_MESSAGE_MAP()
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
public:
	afx_msg void OnOutilsDeplacer();
	afx_msg void OnOutilsEchelle();
	afx_msg void OnOutilsSelection();
	afx_msg void OnOutilsTourner();
	afx_msg void OnOutilsVerrouillersurx();
	afx_msg void OnOutilsVerrouillersury();
	afx_msg void OnOutilsVerrouillersurz();
	afx_msg void OnVueArriere();
	afx_msg void OnVueDeplacer();
	afx_msg void OnVueDessous();
	afx_msg void OnVueDessus();
	afx_msg void OnVueDroite();
	afx_msg void OnVueFace();
	afx_msg void OnVueGauche();
	afx_msg void OnVuePerspective();
	afx_msg void OnVueTourner();
	afx_msg void OnVueProfondeur();
	afx_msg void OnVueZoom();
	afx_msg void OnDestroy();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnCreateMainfire();
	afx_msg void OnCreateSecfire();
	afx_msg void OnCreateBackfire();
	afx_msg void OnCreateDefense();
	afx_msg void OnCreateBombdrop();
};

#ifndef _DEBUG  // version de débogage dans SpaceEditorView.cpp
inline CSpaceEditorDoc* CSpaceEditorView::GetDocument() const
   { return reinterpret_cast<CSpaceEditorDoc*>(m_pDocument); }
#endif

