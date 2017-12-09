// MapObjectPropDlg.cpp : fichier d'implémentation
//

#include "stdafx.h"
#include "DeathEditor.h"
#include "MapObjectPropDlg.h"


// Boîte de dialogue CMapObjectPropDlg

IMPLEMENT_DYNAMIC(CMapObjectPropDlg, CDialog)

CMapObjectPropDlg::CMapObjectPropDlg(Game::IObject *pObject,CWnd* pParent /*=NULL*/)
	: CDialog(CMapObjectPropDlg::IDD, pParent)
	, m_strName(_T(""))
	, m_bCastShadows(FALSE)
	, m_bReceiveShadows(FALSE)
	, m_bCollide(FALSE),
	m_pObject(pObject)
{

}

CMapObjectPropDlg::~CMapObjectPropDlg()
{
}

void CMapObjectPropDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_MOBJ_NAME, m_strName);
	DDX_Check(pDX, IDC_MOBJ_CASTSHADOWS, m_bCastShadows);
	DDX_Check(pDX, IDC_MOBJ_RECSHADOWS, m_bReceiveShadows);
	DDX_Check(pDX, IDC_MOBJ_COLLIDE, m_bCollide);
}


BEGIN_MESSAGE_MAP(CMapObjectPropDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CMapObjectPropDlg::OnBnClickedOk)
	ON_EN_CHANGE(IDC_MOBJ_NAME, &CMapObjectPropDlg::OnEnChangeMobjName)
END_MESSAGE_MAP()


// Gestionnaires de messages de CMapObjectPropDlg

void CMapObjectPropDlg::OnBnClickedOk()
{
	UpdateData(TRUE);

	m_pObject->SetCastShadows(m_bCastShadows ? true : false);
	m_pObject->SetReceiveShadows(m_bReceiveShadows ? true : false);
	m_pObject->SetCollideWithWorld(m_bCollide ? true : false);

	m_pObject->SetName(m_strName);

	OnOK();
}

void CMapObjectPropDlg::OnEnChangeMobjName()
{
	// TODO:  S'il s'agit d'un contrôle RICHEDIT, le contrôle
	// n'enverra la notification que si vous substituez la fonction CDialog::OnInitDialog()
	// et l'appel CRichEditCtrl().SetEventMask()
	// avec l'indicateur ENM_CHANGE ajouté au masque grâce à l'opérateur OR.

	// TODO:  Ajoutez ici le code de votre gestionnaire de notification de contrôle
}

BOOL CMapObjectPropDlg::OnInitDialog()
{
	if(!CDialog::OnInitDialog() || !m_pObject)
		return FALSE;

	m_bCastShadows=m_pObject->CastShadows();
	m_bReceiveShadows=m_pObject->ReceiveShadows();
	m_bCollide=m_pObject->CollideWithWorld();

	m_strName=m_pObject->GetName();

	UpdateData(FALSE);

	return TRUE;
}