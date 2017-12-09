#pragma once


// Boîte de dialogue NewMap

class NewMap : public CDialog
{
	DECLARE_DYNAMIC(NewMap)

public:
	NewMap(CWnd* pParent = NULL);   // constructeur standard
	virtual ~NewMap();

	void Reinit();

// Données de boîte de dialogue
	enum { IDD = IDD_NEWMAP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // Prise en charge de DDX/DDV
	void OnOK();
	void OnCancel();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual BOOL OnInitDialog();
	CString m_strName;
	int m_iSize;
	afx_msg void OnBnClickedNewmapCancel();
	afx_msg void OnBnClickedNewmapNext();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};
