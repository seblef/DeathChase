// MeshObjPropDlg.cpp : fichier d'implémentation
//

#include "stdafx.h"
#include "DeathEditor.h"
#include "MeshObjPropDlg.h"


// Boîte de dialogue CMeshObjPropDlg

IMPLEMENT_DYNAMIC(CMeshObjPropDlg, CDialog)

CMeshObjPropDlg::CMeshObjPropDlg(IMeshObject *pObject, CWnd* pParent /*=NULL*/)
	: CDialog(CMeshObjPropDlg::IDD, pParent)
	, m_bCastShadows(FALSE)
	, m_bReceiveShadows(FALSE)
	, m_bCollide(FALSE)
	, m_strName(_T(""))
	, m_bAnim(FALSE)
	, m_iAxe(0)
	, m_iSpeed(0),
	m_pObject(pObject)
{

}

CMeshObjPropDlg::~CMeshObjPropDlg()
{
}

void CMeshObjPropDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_MESHOBJ_CASTSHADOWS, m_bCastShadows);
	DDX_Check(pDX, IDC_MESHOBJ_RECSHADOWS, m_bReceiveShadows);
	DDX_Check(pDX, IDC_MESHOBJ_COLLIDE, m_bCollide);
	DDX_Text(pDX, IDC_MESHOBJ_NAME, m_strName);
	DDX_Check(pDX, IDC_MESHOBJ_ANIM, m_bAnim);
	DDX_Text(pDX, IDC_MESHOBJ_SPEED, m_iSpeed);
	DDX_Control(pDX, IDC_MESHOBJ_SPEEDSB, m_SpeedSb);
}


BEGIN_MESSAGE_MAP(CMeshObjPropDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CMeshObjPropDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_MESHOBJ_ANIM, &CMeshObjPropDlg::OnBnClickedMeshobjAnim)
	ON_BN_CLICKED(IDC_MESHOBJ_AXEX, &CMeshObjPropDlg::OnBnClickedMeshobjAxex)
	ON_BN_CLICKED(IDC_MESHOBJ_AXEY, &CMeshObjPropDlg::OnBnClickedMeshobjAxey)
	ON_BN_CLICKED(IDC_MESHOBJ_AXEZ, &CMeshObjPropDlg::OnBnClickedMeshobjAxez)
END_MESSAGE_MAP()


// Gestionnaires de messages de CMeshObjPropDlg

BOOL CMeshObjPropDlg::OnInitDialog()
{
	if(!CDialog::OnInitDialog() || !m_pObject)
		return FALSE;

	m_SpeedSb.SetRange(-360,360);

	m_strName=m_pObject->GetName();
	m_bCastShadows=m_pObject->CastShadows();
	m_bReceiveShadows=m_pObject->ReceiveShadows();
	m_bCollide=m_pObject->CollideWithWorld();

	m_bAnim=m_pObject->IsAnimated();

	if(m_bAnim)
	{
		GetDlgItem(IDC_MESHOBJ_AXEX)->EnableWindow(TRUE);
		GetDlgItem(IDC_MESHOBJ_AXEY)->EnableWindow(TRUE);
		GetDlgItem(IDC_MESHOBJ_AXEZ)->EnableWindow(TRUE);
		GetDlgItem(IDC_MESHOBJ_SPEED)->EnableWindow(TRUE);
		m_SpeedSb.EnableWindow(TRUE);

		float fSpeed;
		m_pObject->GetAnimation(m_iAxe,fSpeed);
		m_iSpeed=(int)(fSpeed*180.0f / PI);
	}

	UpdateData(FALSE);

	return TRUE;
}

void CMeshObjPropDlg::OnBnClickedOk()
{
	UpdateData(TRUE);

	m_pObject->SetCastShadows(m_bCastShadows ? true : false);
	m_pObject->SetReceiveShadows(m_bReceiveShadows ? true : false);
	m_pObject->SetCollideWithWorld(m_bCollide ? true : false);
	m_pObject->SetName(m_strName);

	if(m_bAnim)
		m_pObject->SetAnimation(m_iAxe,((float)m_iSpeed) * PI / 180.0f);
	else
		m_pObject->DisableAnimation();

	OnOK();
}

void CMeshObjPropDlg::OnBnClickedMeshobjAnim()
{
	UpdateData(TRUE);
	GetDlgItem(IDC_MESHOBJ_AXEX)->EnableWindow(m_bAnim);
	GetDlgItem(IDC_MESHOBJ_AXEY)->EnableWindow(m_bAnim);
	GetDlgItem(IDC_MESHOBJ_AXEZ)->EnableWindow(m_bAnim);
	GetDlgItem(IDC_MESHOBJ_SPEED)->EnableWindow(m_bAnim);
	m_SpeedSb.EnableWindow(m_bAnim);
}

void CMeshObjPropDlg::OnBnClickedMeshobjAxex()
{
	m_iAxe=0;
}

void CMeshObjPropDlg::OnBnClickedMeshobjAxey()
{
	m_iAxe=1;
}

void CMeshObjPropDlg::OnBnClickedMeshobjAxez()
{
	m_iAxe=2;
}
