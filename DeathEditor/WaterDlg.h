#pragma once
#include "afxwin.h"
#include "afxcmn.h"

#include "../DeathEngine/Core/Handles.h"

// Boîte de dialogue CWaterDlg

class CWaterDlg : public CDialog
{
	DECLARE_DYNAMIC(CWaterDlg)

public:
	CWaterDlg(CWnd* pParent = NULL);   // constructeur standard
	virtual ~CWaterDlg();
	virtual BOOL OnInitDialog();

	void Reinit();

// Données de boîte de dialogue
	enum { IDD = IDD_WATERDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // Prise en charge de DDX/DDV
	void OnOK();
	void OnCancel();

	bool m_bInited;
	bool m_bMinimized;
	Core::HTexture m_hTexture;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedWaterTexturebtn();
	afx_msg void OnEnChangeWaterHeighteb();
	afx_msg void OnBnClickedWaterSurroundck();
	afx_msg void OnBnClickedWaterApplybtn();
	afx_msg void OnBnClickedWaterPrev();
	afx_msg void OnBnClickedWaterNext();
	int m_iHeight;
	BOOL m_bSurround;
	CStatic m_Preview;
	CSpinButtonCtrl m_HeightSb;
};
