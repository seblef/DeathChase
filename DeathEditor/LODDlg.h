#pragma once
#include "afxwin.h"

#include <vector>

class IMesh;
class ICamera;

// Boîte de dialogue CLODDlg

class CLODDlg : public CDialog
{
	DECLARE_DYNAMIC(CLODDlg)

public:
	CLODDlg(CWnd* pParent = NULL);   // constructeur standard
	virtual ~CLODDlg();
	virtual BOOL OnInitDialog();

	void SetMesh(IMesh *pMesh, const char *szPath) { m_pMesh=pMesh; m_strPath=szPath; }

// Données de boîte de dialogue
	enum { IDD = IDD_LODEDITDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // Prise en charge de DDX/DDV

	void Draw();

	bool m_bInited;

	ICamera *m_pCamera;
	IMesh *m_pMesh;
	CString m_strPath;
	std::vector<IMesh *> m_InitialLODs;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	CComboBox m_LODCb;
	DWORD m_dwLODDist;
	CComboBox m_MatCb;
	afx_msg void OnCbnSelchangeLodSelectcb();
	afx_msg void OnCbnSelchangeLodMatcb();
	afx_msg void OnEnChangeLodDisteb();
	afx_msg void OnBnClickedLodNewbtn();
	afx_msg void OnBnClickedLodRembtn();
	afx_msg void OnBnClickedLodMateditbtn();
	afx_msg void OnBnClickedLodMatimportbtn();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOk();
	CStatic m_Preview;
	afx_msg void OnBnClickedLodImport();
};
