

#pragma once
#include "afxwin.h"

#ifdef DEDEV

// Boîte de dialogue CConsole

class CConsole : public CDialog
{
	DECLARE_DYNAMIC(CConsole)

public:
	CConsole(CWnd* pParent = NULL);   // constructeur standard
	virtual ~CConsole();
	virtual BOOL OnInitDialog();

	bool IsCreated() const { return m_bCreated; }
	void AddLine(const char *szLine);

// Données de boîte de dialogue
	enum { IDD = IDD_CONSOLE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // Prise en charge de DDX/DDV
	virtual void OnOK();
	virtual void OnCancel();

	bool m_bCreated;

	DECLARE_MESSAGE_MAP()
public:
	CListBox m_List;
	afx_msg void OnBnClickedConsoleclear();
};

#endif