// Control.cpp : fichier d'implémentation
//

#include "stdafx.h"
#include "DeathEditor.h"
#include "Control.h"

#include "ObjectPaneDlg.h"

// CControl

IMPLEMENT_DYNCREATE(CControl, CFormView)

CControl::CControl()
	: CFormView(CControl::IDD), m_iCurrentEditor(-1)
{

}

CControl::~CControl()
{
}

void CControl::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CONTROLTAB, m_Tab);
}

BEGIN_MESSAGE_MAP(CControl, CFormView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_NOTIFY(TCN_SELCHANGE, IDC_CONTROLTAB, &CControl::OnTcnSelchangeControltab)
END_MESSAGE_MAP()


// Diagnostics de CControl

#ifdef _DEBUG
void CControl::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CControl::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// Gestionnaires de messages de CControl

int CControl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

void CControl::OnUpdate(CView *pSender, LPARAM lHint, CObject *pHint)
{
	CFormView::OnUpdate(pSender,lHint,pHint);

	CDeathEditorDoc *pDoc=(CDeathEditorDoc *)GetDocument();
	if(pDoc->GetCurrentEditor()==-1)
		return;

	if(m_iCurrentEditor!=pDoc->GetCurrentEditor())
	{
		m_Tab.Clear();
		m_iCurrentEditor=pDoc->GetCurrentEditor();

		switch(m_iCurrentEditor)
		{
			case EDITOR_MAP:
				{
					CObjectPaneDlg *pObj=new CObjectPaneDlg(pDoc);
					m_Tab.AddView(pObj,CObjectPaneDlg::IDD,"Objets");

					m_Tab.SetCurrentView(0);
				}
				break;

			default:
				break;
		}
	}

	m_Tab.Update((int)lHint, (void *)pHint);
}


void CControl::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

/*	static int iSizeCount=0;

	iSizeCount++;

	if(iSizeCount < 6)
		return;

	CRect cr;
	GetClientRect(&cr);
	m_Tab.SetWindowPos(0,0,0,cr.Width(),cr.Height(),SWP_NOMOVE | SWP_NOZORDER);*/
}

void CControl::OnTcnSelchangeControltab(NMHDR *pNMHDR, LRESULT *pResult)
{
	m_Tab.SetCurrentView(m_Tab.GetCurSel());

	*pResult = 0;
}
