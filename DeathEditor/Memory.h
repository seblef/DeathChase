

#pragma once
#include "afxcmn.h"
#include "afxwin.h"

#ifdef DEDEV

// Boîte de dialogue CMemory

class CMemory : public CDialog
{
	DECLARE_DYNAMIC(CMemory)

public:
	CMemory(CWnd* pParent = NULL);   // constructeur standard
	virtual ~CMemory();
	virtual BOOL OnInitDialog();

// Données de boîte de dialogue
	enum { IDD = IDD_MEMORY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // Prise en charge de DDX/DDV
	virtual void OnOK();
	virtual void OnCancel();

	const char *FormatSize(const int iSize);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedMemrefresh();
	afx_msg void OnTcnSelchangeMemtab(NMHDR *pNMHDR, LRESULT *pResult);
	CTabCtrl m_Tab;
	CListBox m_AllocList;
	CListCtrl m_MemList;
	afx_msg void OnLbnSelchangeAlloclist();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};

#endif