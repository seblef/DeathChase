// LODDlg.cpp : fichier d'implémentation
//

#include "stdafx.h"
#include "DeathEditor.h"
#include "LODDlg.h"

#include "FileSelectDlg.h"
#include "MeshEditDlg.h"

#include "Interfaces.h"
#include "../DeathEngine/Export/IMeshMgr.h"
#include "../DeathEngine/Export/IMesh.h"
#include "../DeathEngine/Export/IRenderer.h"
#include "../DeathEngine/Export/IFactory.h"
#include "../DeathEngine/Export/ICamera.h"

// Boîte de dialogue CLODDlg

IMPLEMENT_DYNAMIC(CLODDlg, CDialog)

CLODDlg::CLODDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLODDlg::IDD, pParent),
	m_pMesh(0),
	m_pCamera(0),
	m_bInited(false)
	, m_dwLODDist(0)
{

}

CLODDlg::~CLODDlg()
{
	if(m_pCamera)
		g_Factory->ReleaseObject(m_pCamera);
}

void CLODDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LOD_SELECTCB, m_LODCb);
	DDX_Text(pDX, IDC_LOD_DISTEB, m_dwLODDist);
	DDX_Control(pDX, IDC_LOD_MATCB, m_MatCb);
	DDX_Control(pDX, IDC_LOD_PREVIEW, m_Preview);
}


BEGIN_MESSAGE_MAP(CLODDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_SHOWWINDOW()
	ON_WM_TIMER()
	ON_CBN_SELCHANGE(IDC_LOD_SELECTCB, &CLODDlg::OnCbnSelchangeLodSelectcb)
	ON_CBN_SELCHANGE(IDC_LOD_MATCB, &CLODDlg::OnCbnSelchangeLodMatcb)
	ON_EN_CHANGE(IDC_LOD_DISTEB, &CLODDlg::OnEnChangeLodDisteb)
	ON_BN_CLICKED(IDC_LOD_NEWBTN, &CLODDlg::OnBnClickedLodNewbtn)
	ON_BN_CLICKED(IDC_LOD_REMBTN, &CLODDlg::OnBnClickedLodRembtn)
	ON_BN_CLICKED(IDC_LOD_MATEDITBTN, &CLODDlg::OnBnClickedLodMateditbtn)
	ON_BN_CLICKED(IDC_LOD_MATIMPORTBTN, &CLODDlg::OnBnClickedLodMatimportbtn)
	ON_BN_CLICKED(IDCANCEL, &CLODDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, &CLODDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_LOD_IMPORT, &CLODDlg::OnBnClickedLodImport)
END_MESSAGE_MAP()


// Gestionnaires de messages de CLODDlg

BOOL CLODDlg::OnInitDialog()
{
	if(!CDialog::OnInitDialog() || !m_pMesh)
		return FALSE;

	CString strLOD;

	for(int i=0;i<m_pMesh->GetLODCount();i++)
	{
		strLOD.Format("LOD %i",i);
		m_LODCb.AddString(strLOD);
		m_InitialLODs.push_back(m_pMesh->GetLODByIndex(i));
	}

	m_pCamera=(ICamera *)g_Factory->CreateObject(EOBJ_CAMERA);

	m_bInited=true;

	return TRUE;
}


void CLODDlg::OnPaint()
{
	CPaintDC dc(this);

	Draw();
}

void CLODDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	if(bShow)
	{
		CRect rect;
		m_Preview.GetClientRect(&rect);
		g_Renderer->AddView(m_Preview.m_hWnd,rect.Width(),rect.Height());
		m_LODCb.SetCurSel(0);
		OnCbnSelchangeLodSelectcb();
		Draw();
	}
	else
		g_Renderer->RemoveView(m_Preview.m_hWnd);
}

void CLODDlg::OnTimer(UINT_PTR nIDEvent)
{
	if(nIDEvent==1)
	{
		Core::Vector3 vFromTarget(m_pCamera->GetPosition() - m_pCamera->GetTarget());
		Core::Quaternion qY(Core::Vector3::YAxisVector,0.04f);
		vFromTarget=qY * vFromTarget;

		m_pCamera->SetPosition(m_pCamera->GetTarget() + vFromTarget);
		Draw();
	}

	CDialog::OnTimer(nIDEvent);
}

void CLODDlg::OnCbnSelchangeLodSelectcb()
{
	const int iSelected=m_LODCb.GetCurSel();
	if(iSelected==-1 || !m_bInited) return;

	IMesh *pLod=m_pMesh->GetLODByIndex(iSelected);

	Core::BBox bbox(pLod->GetBBox());

	Core::Vector3 vSize(bbox.m_vMax - bbox.m_vMin);
	const float fMaxSize=MAX(MAX(fabsf(vSize.x),fabsf(vSize.y)),fabsf(vSize.z));

	const Core::Vector3 vTarget((bbox.m_vMax + bbox.m_vMin)*0.5f);

	CRect rect;
	m_Preview.GetClientRect(&rect);
	const float fZMax=fMaxSize*2.0f;
	m_pCamera->Initialize(CAM_PERSPECTIVE,fZMax * 0.001f,fZMax,(float)rect.Width()/2,(float)rect.Height()/2,90,1.0f);
			
	m_pCamera->SetTarget(vTarget);
	m_pCamera->SetPosition(Core::Vector3(vTarget.x,vTarget.y,vTarget.z - fMaxSize*1.25f));

	m_dwLODDist=pLod->GetMaxLODDistance();
	UpdateData(FALSE);

	this->SetTimer(1,50,0);
}

void CLODDlg::OnCbnSelchangeLodMatcb()
{
	// TODO : ajoutez ici le code de votre gestionnaire de notification de contrôle
}

void CLODDlg::OnEnChangeLodDisteb()
{
	const int iSelected=m_LODCb.GetCurSel();
	if(!m_bInited || iSelected==-1) return;

	UpdateData(TRUE);
	m_pMesh->GetLODByIndex(iSelected)->SetMaxLODDistance(m_dwLODDist*m_dwLODDist);
}

void CLODDlg::OnBnClickedLodNewbtn()
{
	CFileSelectDlg dlg;
	dlg.SetFileType(FILE_MESH);

	if(dlg.DoModal()==IDCANCEL) return;

	IMesh *pLod=m_pMesh->CreateLOD(0);
	if(!pLod->Load(dlg.m_strFileName,dlg.m_strPathName))
	{
		m_pMesh->RemoveLOD(m_pMesh->GetLODCount()-1);
		MessageBox("Erreur au chargement de l'objet.","Erreur",MB_OK | MB_ICONERROR);
		return;
	}

	CString strLODName;
	strLODName.Format("LOD %i",m_pMesh->GetLODCount());
	m_LODCb.AddString(strLODName);

	m_LODCb.SetCurSel(m_LODCb.GetCount()-1);
	OnCbnSelchangeLodSelectcb();
}

void CLODDlg::OnBnClickedLodRembtn()
{
	const int iSelected=m_LODCb.GetCurSel();
	if(iSelected==-1) return;

	if(iSelected==0)
	{
		MessageBox("Vous ne pouvez pas supprimer le premier LOD.","Info",MB_OK | MB_ICONINFORMATION);
		return;
	}

	m_LODCb.SetCurSel(0);
	OnCbnSelchangeLodSelectcb();

	m_pMesh->RemoveLOD(iSelected);
	m_LODCb.DeleteString(iSelected);
}

void CLODDlg::OnBnClickedLodMateditbtn()
{
	// TODO : ajoutez ici le code de votre gestionnaire de notification de contrôle
}

void CLODDlg::OnBnClickedLodMatimportbtn()
{
	// TODO : ajoutez ici le code de votre gestionnaire de notification de contrôle
}

void CLODDlg::OnBnClickedCancel()
{
	if(m_pMesh)
	{
		std::vector<int> indexes;

		for(int j=0;j<m_pMesh->GetLODCount();j++)
		{
			bool bInitial=false;
			IMesh *pLod=m_pMesh->GetLODByIndex(j);
			for(int i=0;i<m_InitialLODs.size();i++)
			{
				if(m_InitialLODs[i]==pLod)
				{
					bInitial=true;
					break;
				}
			}

			if(!bInitial) indexes.push_back(j);
		}

		for(int i=0;i<indexes.size();i++)
			m_pMesh->RemoveLOD(indexes[i]);
	}

	OnCancel();
}

void CLODDlg::OnBnClickedOk()
{
	// TODO : ajoutez ici le code de votre gestionnaire de notification de contrôle
	OnOK();
}

void CLODDlg::Draw()
{
	const int iSelected=m_LODCb.GetCurSel();

	if(m_bInited && iSelected!=-1)
	{
		g_Renderer->SetView(m_Preview.m_hWnd);
		g_Renderer->BeginScene();

		g_Renderer->SetTransformation(TRANS_VIEWPROJ,m_pCamera->GetViewProjMatrix());
		g_Renderer->SetVertexDeclaration(Core::VX_PNDTT);

		m_pMesh->GetLODByIndex(iSelected)->Draw(Core::Matrix4::Identity,0,Core::Vector3::NullVector);

		g_Renderer->EndScene();
	}
}

void CLODDlg::OnBnClickedLodImport()
{
	CMeshEditDlg dlg(true);
	dlg.m_pMesh=m_pMesh->CreateLOD(0);

	if(dlg.DoModal()==IDCANCEL)
		m_pMesh->RemoveLOD(m_pMesh->GetLODCount()-1);

	CString strLODName;
	strLODName.Format("LOD %i",m_pMesh->GetLODCount()-1);
	m_LODCb.AddString(strLODName);

	m_LODCb.SetCurSel(m_LODCb.GetCount()-1);
	OnCbnSelchangeLodSelectcb();
}
