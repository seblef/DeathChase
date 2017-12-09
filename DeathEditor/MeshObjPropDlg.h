#pragma once
#include "afxcmn.h"

#include "../DeathEngine/Export/IMeshObject.h"


// Boîte de dialogue CMeshObjPropDlg

class CMeshObjPropDlg : public CDialog
{
	DECLARE_DYNAMIC(CMeshObjPropDlg)

public:
	CMeshObjPropDlg(IMeshObject *pObject, CWnd* pParent = NULL);   // constructeur standard
	virtual ~CMeshObjPropDlg();
	BOOL OnInitDialog();

// Données de boîte de dialogue
	enum { IDD = IDD_MESHOBJ_PROPDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // Prise en charge de DDX/DDV

	IMeshObject *m_pObject;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedMeshobjAnim();
	BOOL m_bCastShadows;
	BOOL m_bReceiveShadows;
	BOOL m_bCollide;
	CString m_strName;
	BOOL m_bAnim;
	int m_iAxe;
	int m_iSpeed;
	CSpinButtonCtrl m_SpeedSb;
	afx_msg void OnBnClickedMeshobjAxex();
	afx_msg void OnBnClickedMeshobjAxey();
	afx_msg void OnBnClickedMeshobjAxez();
};
