#pragma once
#include "afxwin.h"
#include "afxcmn.h"

#include "../DeathEngine/Core/Handles.h"
#include "Data/TexGenLevel.h"
#include <vector>

// Boîte de dialogue CTextureDlg

class CTextureDlg : public CDialog
{
	DECLARE_DYNAMIC(CTextureDlg)

public:
	CTextureDlg(CWnd* pParent = NULL);   // constructeur standard
	virtual ~CTextureDlg();
	virtual BOOL OnInitDialog();

	void Reinit();

// Données de boîte de dialogue
	enum { IDD = IDD_TEXTUREDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // Prise en charge de DDX/DDV
	void OnOK();
	void OnCancel();

	void EnableParameters(BOOL bEnable);
	void EnableTexParameters(BOOL bEnable);

	bool m_bInited;
	bool m_bMinimized;
	Core::HTexture m_hTexture;
	Core::HTexture m_hCurrentTexture;
	
	std::vector<TexGenLevel> m_TexLevels;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedTexGenerateck();
	afx_msg void OnBnClickedTexFilebtn();
	afx_msg void OnCbnSelchangeTexSizecb();
	afx_msg void OnLbnSelchangeTexTexlist();
	afx_msg void OnBnClickedTexAddbtn();
	afx_msg void OnBnClickedTexRembtn();
	afx_msg void OnEnChangeTexMineb();
	afx_msg void OnEnChangeTexMaxeb();
	afx_msg void OnBnClickedTexTexbtn();
	afx_msg void OnBnClickedTexGeneratebtn();
	afx_msg void OnBnClickedTexDetailbtn();
	afx_msg void OnBnClickedTexPrevbtn();
	afx_msg void OnBnClickedTexNextbtn();
	BOOL m_bGenerate;
	int m_iSize;
	CListBox m_TexList;
	int m_iMin;
	int m_iMax;
	CStatic m_Preview;
	CSpinButtonCtrl m_MinSb;
	CSpinButtonCtrl m_MaxSb;
	afx_msg void OnStnClickedTexPreview();
};
