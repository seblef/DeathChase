#pragma once
#include "afxwin.h"
#include "afxcmn.h"

#include "../DeathEngine/Core/Handles.h"

class IHeightMapGenerator;

// Boîte de dialogue CHeightmapDlg

class CHeightmapDlg : public CDialog
{
	DECLARE_DYNAMIC(CHeightmapDlg)

public:
	CHeightmapDlg(CWnd* pParent = NULL);   // constructeur standard
	virtual ~CHeightmapDlg();
	virtual BOOL OnInitDialog();
	void Reinit();

// Données de boîte de dialogue
	enum { IDD = IDD_HEIGHTMAPDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // Prise en charge de DDX/DDV
	virtual void OnOK();
	virtual void OnCancel();

	void EnableHeightmapParameters(BOOL bEnable);
	void ShowHParams(IHeightMapGenerator *pGenerator);
	void FillTexture(bool bReleasePatchTex);

	int m_iHParamCount;
	float *m_pHGeneratorValues[5];
	Core::HTexture m_hTexture;
	IHeightMapGenerator *m_pCurrentGenerator;
	bool m_bMinimized;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedHgtGenerateck();
	afx_msg void OnBnClickedHgtFilebtn();
	afx_msg void OnBnClickedHgtGeneratebtn();
	afx_msg void OnBnClickedHgtSmoothbtn();
	afx_msg void OnBnClickedHgtScalebtn();
	afx_msg void OnBnClickedHgtPrev();
	afx_msg void OnBnClickedHgtNext();
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnCbnSelchangeHgtGentypecb();
	int m_iGenType;
	CEdit m_ParamEb[5];
	CStatic m_ParamSt[5];
	CSpinButtonCtrl m_ParamSb[5];
	int m_iScale;
	CSpinButtonCtrl m_ScaleSb;
	BOOL m_bGenerate;
	CStatic m_Preview;
};
