#pragma once
#include "afxwin.h"
#include "afxcmn.h"

#include "MFC/btnst.h"


// Boîte de dialogue CEnvironmentDlg

class CEnvironmentDlg : public CDialog
{
	DECLARE_DYNAMIC(CEnvironmentDlg)

public:
	CEnvironmentDlg(CWnd* pParent = NULL);   // constructeur standard
	virtual ~CEnvironmentDlg();
	virtual BOOL OnInitDialog();
	void Reinit();

// Données de boîte de dialogue
	enum { IDD = IDD_ENVIRONMENTDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // Prise en charge de DDX/DDV
	void OnOK();
	void OnCancel();

	void Draw();

	bool m_bMinimized;
	bool m_bInited;
	int m_iCurrentDraw;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLbnSelchangeEnvSkylist();
	afx_msg void OnBnClickedEnvSkytexbtn();
	afx_msg void OnCbnSelchangeEnvWeatherTypecb();
	afx_msg void OnEnChangeEnvWeatherDensityeb();
	afx_msg void OnBnClickedEnvWeatherTexbtn();
	afx_msg void OnBnClickedEnvSunTexbtn();
	afx_msg void OnBnClickedEnvSunColorbtn();
	afx_msg void OnBnClickedEnvGeneratewaterck();
	afx_msg void OnBnClickedEnvGeneratetreesck();
	afx_msg void OnBnClickedEnvPrevbtn();
	afx_msg void OnBnClickedEnvNextbtn();
	CListBox m_SkyList;
	int m_iWeatherType;
	int m_iWeatherDensity;
	BOOL m_bGenerateWater;
	BOOL m_bGenerateTrees;
	CStatic m_Preview;
	CSpinButtonCtrl m_WeatherDensitySb;
	afx_msg void OnBnClickedEnvShieldTexbtn();
	afx_msg void OnEnChangeEnvShieldDisteb();
	DWORD m_dwShieldDist;
	CSpinButtonCtrl m_ShieldSb;
	CButtonST m_AmbientBtn;
	afx_msg void OnBnClickedEnvAmbientbtn();
	CButtonST m_SunColorBtn;
};
