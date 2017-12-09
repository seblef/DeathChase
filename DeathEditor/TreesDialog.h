#pragma once

#include "MFC/CStaticView.h"
#include "afxwin.h"
#include "afxcmn.h"

#include "Data/TreePlacement.h"
#include "../DeathEngine/Core/Handles.h"

class ICamera;

// Boîte de dialogue CTreesDialog

class CTreesDialog : public CDialog
{
	DECLARE_DYNAMIC(CTreesDialog)

public:
	CTreesDialog(CWnd* pParent = NULL);   // constructeur standard
	virtual ~CTreesDialog();
	virtual BOOL OnInitDialog();

	void Reinit();

// Données de boîte de dialogue
	enum { IDD = IDD_TREES };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // Prise en charge de DDX/DDV
	virtual void OnOK();
	virtual void OnCancel();

	class TreeItem
	{
		public:

			Core::HMesh hMesh;
			int iMin,iMax;
			float fScale;
			char szName[64];
			bool m_bSelected;

	};

	std::vector<TreeItem> m_Trees;
	bool m_bInited;

	TreePlacement m_Placement;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedTreeimport();
	afx_msg void OnBnClickedTreeapply();
	afx_msg void OnLbnSelchangeTreelist();
	CListBox m_List;
	CStaticMeshView m_Preview;
	int m_iMin;
	int m_iMax;
	CSpinButtonCtrl m_MinSb;
	CSpinButtonCtrl m_MaxSb;
	int m_iDensity;
	CSpinButtonCtrl m_DensitySb;
	afx_msg void OnEnChangeTreemineb();
	afx_msg void OnEnChangeTreemaxeb();
	afx_msg void OnEnChangeTreedensityeb();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnDestroy();
	afx_msg void OnLbnDblclkTreelist();
	int m_iScale;
	CSpinButtonCtrl m_ScaleSb;
	BOOL m_bSelected;
	afx_msg void OnBnClickedTreeselectck();
	afx_msg void OnEnChangeTreescaleeb();
	afx_msg void OnBnClickedTreesPrev();
	afx_msg void OnBnClickedTreesNext();
};
