#pragma once
#include "afxcmn.h"


// Boîte de dialogue CShipPropDlg

class CShipPropDlg : public CDialog
{
	DECLARE_DYNAMIC(CShipPropDlg)

public:
	CShipPropDlg(CWnd* pParent = NULL);   // constructeur standard
	virtual ~CShipPropDlg();

// Données de boîte de dialogue
	enum { IDD = IDD_SHIPPROP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // Prise en charge de DDX/DDV
	virtual BOOL OnInitDialog();

	static CShipPropDlg *m_pInstance;
	static bool m_sbShown;

	bool m_bInitialized;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnEnChangeShippropName();
	afx_msg void OnEnChangeShippropTspeedeb();
	afx_msg void OnEnChangeShippropRspeedeb();
	afx_msg void OnBnClickedShippropSound();
	afx_msg void OnBnClickedShippropPlaysound();
	CString m_strName;
	int m_iTSpeed;
	int m_iRSpeed;
	CSpinButtonCtrl m_TSpeedSb;
	CSpinButtonCtrl m_RSpeedSb;

	static void Show();
	static void Hide();
	static bool IsShown() { return m_sbShown; }
	static CShipPropDlg *Instance() { return m_pInstance; }
};
