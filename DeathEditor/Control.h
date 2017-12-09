#pragma once
#include "afxcmn.h"
#include "MFC/CTabDlg.h"



// Mode formulaire CControl

class CControl : public CFormView
{
	DECLARE_DYNCREATE(CControl)

protected:
	CControl();           // constructeur protégé utilisé par la création dynamique
	virtual ~CControl();

	int m_iCurrentEditor;

public:
	enum { IDD = IDD_CONTROL };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // Prise en charge de DDX/DDV
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);

	DECLARE_MESSAGE_MAP()
public:
	CTabDlg m_Tab;
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTcnSelchangeControltab(NMHDR *pNMHDR, LRESULT *pResult);
};


