// ShipPropDlg.cpp : fichier d'implémentation
//

#include "stdafx.h"
#include "SpaceEditor.h"
#include "ShipPropDlg.h"


// Boîte de dialogue CShipPropDlg

IMPLEMENT_DYNAMIC(CShipPropDlg, CDialog)


CShipPropDlg *CShipPropDlg::m_pInstance=0;
bool CShipPropDlg::m_sbShown=false;


CShipPropDlg::CShipPropDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CShipPropDlg::IDD, pParent)
	, m_strName(_T(""))
	, m_iTSpeed(0)
	, m_iRSpeed(0),
	m_bInitialized(false)
{

}

CShipPropDlg::~CShipPropDlg()
{
}

void CShipPropDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_SHIPPROP_NAME, m_strName);
	DDX_Text(pDX, IDC_SHIPPROP_TSPEEDEB, m_iTSpeed);
	DDV_MinMaxInt(pDX, m_iTSpeed, 1, 150);
	DDX_Text(pDX, IDC_SHIPPROP_RSPEEDEB, m_iRSpeed);
	DDV_MinMaxInt(pDX, m_iRSpeed, 1, 270);
	DDX_Control(pDX, IDC_SHIPPROP_TSPEEDSB, m_TSpeedSb);
	DDX_Control(pDX, IDC_SHIPPROP_RSPEEDSB2, m_RSpeedSb);
}


BEGIN_MESSAGE_MAP(CShipPropDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CShipPropDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CShipPropDlg::OnBnClickedCancel)
	ON_EN_CHANGE(IDC_SHIPPROP_NAME, &CShipPropDlg::OnEnChangeShippropName)
	ON_EN_CHANGE(IDC_SHIPPROP_TSPEEDEB, &CShipPropDlg::OnEnChangeShippropTspeedeb)
	ON_EN_CHANGE(IDC_SHIPPROP_RSPEEDEB, &CShipPropDlg::OnEnChangeShippropRspeedeb)
	ON_BN_CLICKED(IDC_SHIPPROP_SOUND, &CShipPropDlg::OnBnClickedShippropSound)
	ON_BN_CLICKED(IDC_SHIPPROP_PLAYSOUND, &CShipPropDlg::OnBnClickedShippropPlaysound)
END_MESSAGE_MAP()


// Gestionnaires de messages de CShipPropDlg

void CShipPropDlg::OnBnClickedOk()
{
}

void CShipPropDlg::OnBnClickedCancel()
{
}

BOOL CShipPropDlg::OnInitDialog()
{
	if(!CDialog::OnInitDialog()) return FALSE;

	m_TSpeedSb.SetRange(1,150);
	m_RSpeedSb.SetRange(1,270);
	m_bInitialized=true;

	return TRUE;
}

void CShipPropDlg::OnEnChangeShippropName()
{
	if(m_bInitialized)
		UpdateData(TRUE);
}

void CShipPropDlg::OnEnChangeShippropTspeedeb()
{
	if(m_bInitialized)
		UpdateData(TRUE);
}

void CShipPropDlg::OnEnChangeShippropRspeedeb()
{
	if(m_bInitialized)
		UpdateData(TRUE);
}

void CShipPropDlg::OnBnClickedShippropSound()
{
	// TODO : ajoutez ici le code de votre gestionnaire de notification de contrôle
}

void CShipPropDlg::OnBnClickedShippropPlaysound()
{
	// TODO : ajoutez ici le code de votre gestionnaire de notification de contrôle
}

void CShipPropDlg::Show()
{
	if(!m_pInstance)
	{
		m_pInstance=new CShipPropDlg;
		m_pInstance->Create(IDD_SHIPPROP);
	}

	if(m_pInstance)
	{
		m_pInstance->ShowWindow(SW_SHOW);
		m_sbShown=true;
	}
}

void CShipPropDlg::Hide()
{
	if(m_pInstance)
	{
		m_pInstance->ShowWindow(SW_HIDE);
		m_sbShown=false;
	}
}
