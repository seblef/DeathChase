#pragma once

#include "../DeathEngine/Game/IObject.h"


// Boîte de dialogue CMapObjectPropDlg

class CMapObjectPropDlg : public CDialog
{
	DECLARE_DYNAMIC(CMapObjectPropDlg)

public:
	CMapObjectPropDlg(Game::IObject *pObject, CWnd* pParent = NULL);   // constructeur standard
	virtual ~CMapObjectPropDlg();
	BOOL OnInitDialog();

// Données de boîte de dialogue
	enum { IDD = IDD_MOBJECT_PROPDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // Prise en charge de DDX/DDV

	Game::IObject *m_pObject;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnEnChangeMobjName();
	CString m_strName;
	BOOL m_bCastShadows;
	BOOL m_bReceiveShadows;
	BOOL m_bCollide;
};
