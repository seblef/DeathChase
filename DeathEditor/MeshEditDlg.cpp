// MeshEditDlg.cpp : fichier d'implémentation
//

#include "stdafx.h"
#include "DeathEditor.h"
#include "MeshEditDlg.h"
#include "LODDlg.h"

#include "FileSystem.h"
#include "Interfaces.h"

#include "Data/MeshLoader.h"

#include "../../Loaders/SDK/Iloader.h"
#include "../DeathEngine/Core/TDynamicTab.h"

#include "../DeathEngine/Export/IFactory.h"
#include "../DeathEngine/Export/ICamera.h"
#include "../DeathEngine/Export/IRenderer.h"
#include "../DeathEngine/Export/IShaderMgr.h"
#include "../DeathEngine/Export/ITextureMgr.h"
#include "../DeathEngine/Export/IMeshMgr.h"
#include "../DeathEngine/Export/IMesh.h"
#include "../DeathEngine/Export/IMaterialMgr.h"
#include "../DeathEngine/Export/IMaterial.h"

#define MAX_MESH_COUNT					64

#define SMAP_SPECULAR					(1)
#define SMAP_EMISSIVE					(1 << 1)
#define SMAP_LIGHTED					(1 << 2)

#define SMAP_TEXBASE					(1 << 3)
#define SMAP_TEXSECADD					(1 << 4)
#define SMAP_TEXSECFILTER				(1 << 5)
#define SMAP_TEXSECFILTER2X				(1 << 6)

#define SMAP_BUMP						(1 << 7)
#define SMAP_ENVIRONMENT				(1 << 8)

// Boîte de dialogue CMeshEditDlg

IMPLEMENT_DYNAMIC(CMeshEditDlg, CDialog)

CMeshEditDlg::CMeshEditDlg(bool bImportLOD, CWnd* pParent /*=NULL*/)
	: CDialog(CMeshEditDlg::IDD, pParent)
	, m_bImportOnlyMesh(FALSE)
	, m_iMultiAction(1)
	, m_dwLODDist(0),
	m_bImportLOD(bImportLOD),
	m_pLoader(0),
	m_pCamera(0),
	m_bInited(false),
	m_pMesh(0)
	, m_strName(_T(""))
	, m_bImportAnim(FALSE)
	, m_bLODManage(FALSE)
	, m_bCenter(TRUE)
{

}

CMeshEditDlg::~CMeshEditDlg()
{
	if(m_pCamera) g_Factory->ReleaseObject(m_pCamera);
}

void CMeshEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MEDIT_MESHSELECTCB, m_MeshCb);
	DDX_Radio(pDX, IDC_MEDIT_MULTISELECTRD, m_bImportOnlyMesh);
	DDX_Text(pDX, IDC_MEDIT_LODDISTEB, m_dwLODDist);
	DDX_Control(pDX, IDC_MEDIT_MATSELECTCB, m_MatCb);
	DDX_Control(pDX, IDC_MEDITPREVIEW, m_Preview);
	DDX_Text(pDX, IDC_MEDIT_NAME, m_strName);
	DDX_Check(pDX, IDC_MEDIT_ANIMCK, m_bImportAnim);
	DDX_Check(pDX, IDC_MEDIT_LODCK, m_bLODManage);
	DDX_Check(pDX, IDC_MEDIT_CENTERCK, m_bCenter);
}


BEGIN_MESSAGE_MAP(CMeshEditDlg, CDialog)
	ON_WM_CREATE()
	ON_WM_SHOWWINDOW()
	ON_WM_TIMER()
	ON_CBN_SELCHANGE(IDC_MEDIT_MESHSELECTCB, &CMeshEditDlg::OnCbnSelchangeMeditMeshselectcb)
	ON_BN_CLICKED(IDC_MEDIT_MULTISELECTRD, &CMeshEditDlg::OnBnClickedMeditMultiselectrd)
	ON_BN_CLICKED(IDC_MEDIT_MULTISELECTRD2, &CMeshEditDlg::OnBnClickedMeditMultiselectrd2)
	ON_BN_CLICKED(IDC_MEDIT_MULTISELECTRD3, &CMeshEditDlg::OnBnClickedMeditMultiselectrd3)
	ON_EN_CHANGE(IDC_MEDIT_LODDISTEB, &CMeshEditDlg::OnEnChangeMeditLoddisteb)
	ON_CBN_SELCHANGE(IDC_MEDIT_MATSELECTCB, &CMeshEditDlg::OnCbnSelchangeMeditMatselectcb)
	ON_BN_CLICKED(IDC_MEDIT_EDITMATBTN, &CMeshEditDlg::OnBnClickedMeditEditmatbtn)
	ON_BN_CLICKED(IDOK, &CMeshEditDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CMeshEditDlg::OnBnClickedCancel)
	ON_EN_CHANGE(IDC_MEDIT_NAME, &CMeshEditDlg::OnEnChangeMeditName)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_MEDIT_ANIMCK, &CMeshEditDlg::OnBnClickedMeditAnimck)
	ON_BN_CLICKED(IDC_MEDIT_LODCK, &CMeshEditDlg::OnBnClickedMeditLodck)
	ON_BN_CLICKED(IDC_MEDIT_IMPORTMATBTN, &CMeshEditDlg::OnBnClickedMeditImportmatbtn)
	ON_BN_CLICKED(IDC_MEDIT_CENTERCK, &CMeshEditDlg::OnBnClickedMeditCenterck)
END_MESSAGE_MAP()


// Gestionnaires de messages de CMeshEditDlg

BOOL CMeshEditDlg::OnInitDialog()
{
	if(!CDialog::OnInitDialog() || !m_pLoader)
		return FALSE;

	if(m_bImportLOD)
	{
		GetDlgItem(IDC_MEDIT_MULTISELECTRD3)->EnableWindow(FALSE);
		GetDlgItem(IDC_MEDIT_LODDISTEB)->EnableWindow(FALSE);
		GetDlgItem(IDC_MEDIT_LODCK)->EnableWindow(FALSE);
	}

	const int iMeshCount=m_pLoader->GetMeshCount();
	const int iMatCount=m_pLoader->GetMaterialCount();

	if(iMeshCount==0)
	{
		MessageBox("Le fichier ne contient aucun objet!","Erreur",MB_OK | MB_ICONERROR);
		return FALSE;
	}

	if(iMatCount==0)
	{
		IMaterial *pMat=g_MaterialMgr->CreateMaterialWithoutRegistering();
		m_Materials.push_back(pMat);
		m_MatCb.AddString("null");
	}
	else
	{
		for(int i=0;i<iMatCount;i++)
		{
			IMaterial *pMat=0;
			CString strMatName(m_pLoader->GetMaterial(i)->m_szName);
			Core::HMaterial hMat=g_MaterialMgr->GetMaterialByName(strMatName);
			if(!hMat.IsNull())
			{
				CString msg;
				msg.Format("Un matériau portant le nom %s existe déjà. Voulez vous l'utiliser pour cet objet?",strMatName);
				if(MessageBox(msg,"Matériau",MB_YESNO | MB_ICONQUESTION)==IDYES)
				{
					pMat=g_MaterialMgr->GetMaterial(hMat);
					m_Materials.push_back(pMat);
					m_MatCb.AddString(pMat->GetName());
					continue;
				}
				else
				{
					int number=1;
					while(1)
					{
						strMatName.Format("%s%i",m_pLoader->GetMaterial(i)->m_szName,number);
						hMat=g_MaterialMgr->GetMaterialByName(strMatName);
						if(hMat.IsNull())
							break;
					}
				}
			}

			pMat=g_MaterialMgr->CreateMaterialWithoutRegistering();
			ConvertMaterial(pMat,m_pLoader->GetMaterial(i));
			pMat->SetName(strMatName);

			m_Materials.push_back(pMat);
			m_MatCb.AddString(pMat->GetName());
		}
	}

	m_MeshCb.AddString("Tout");
	for(int i=0;i<iMeshCount;i++)
	{
		m_MeshCb.AddString(m_pLoader->GetMeshName(i));
		m_LODDistances.push_back(0);
	}

	m_MatCb.SetCurSel(0);
	m_MeshCb.SetCurSel(0);

	if(iMeshCount <= 1)
	{
		GetDlgItem(IDC_MEDIT_MULTISELECTRD)->EnableWindow(FALSE);
		GetDlgItem(IDC_MEDIT_MULTISELECTRD2)->EnableWindow(FALSE);
		GetDlgItem(IDC_MEDIT_MULTISELECTRD3)->EnableWindow(FALSE);
	}

	m_strName=m_pLoader->GetMeshName(0);
	UpdateData(FALSE);

	m_bInited=true;

	return TRUE;
}
int CMeshEditDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	g_FileSystem.ChangeWorkingDirectory("Models\\");
	CString strPath(g_FileSystem.GetWorkingDirectory());
	strPath+=g_MeshLoader.GetExtPath().c_str();

	CFileDialog dialog(TRUE,"*.*",strPath,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,g_MeshLoader.GetExtensions().c_str());
	if(dialog.DoModal()==IDCANCEL)
	{
		g_FileSystem.ResetWorkingDirectory();
		return -1;
	}

	m_strPath=g_FileSystem.GetWorkingDirectory();
	m_strPath+="\\";
	m_strFileName=dialog.GetPathName();
	
	m_pLoader=g_MeshLoader.LoadMesh(dialog.GetPathName(),0,dialog.GetFileExt());

	g_FileSystem.ResetWorkingDirectory();
	if(!m_pLoader || m_pLoader->GetMeshCount()==0) return -1;

	m_pCamera=(ICamera *)g_Factory->CreateObject(EOBJ_CAMERA);
	return 0;
}

void CMeshEditDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	if(bShow)
	{
		CRect rect;
		m_Preview.GetClientRect(&rect);
		g_Renderer->AddView(m_Preview.m_hWnd,rect.Width(),rect.Height());
		m_MeshCb.SetCurSel(0);
		OnCbnSelchangeMeditMeshselectcb();
		Draw();
	}
	else
		g_Renderer->RemoveView(m_Preview.m_hWnd);
}

void CMeshEditDlg::OnTimer(UINT_PTR nIDEvent)
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

void CMeshEditDlg::OnCbnSelchangeMeditMeshselectcb()
{
	const int iSelected=m_MeshCb.GetCurSel();
	Core::BBox bbox;
	bbox.Init();

	if(iSelected<=0)
	{
		for(int i=0;i<m_pLoader->GetMeshCount();i++)
			bbox.Adjust(m_pLoader->GetMeshBBox(i));
	}
	else
		bbox.Adjust(m_pLoader->GetMeshBBox(iSelected-1));

	Core::Vector3 vSize(bbox.m_vMax - bbox.m_vMin);
	const float fMaxSize=MAX(MAX(fabsf(vSize.x),fabsf(vSize.y)),fabsf(vSize.z));

	const Core::Vector3 vTarget((bbox.m_vMax + bbox.m_vMin)*0.5f);

	CRect rect;
	m_Preview.GetClientRect(&rect);
	const float fZMax=fMaxSize*5.0f;
	m_pCamera->Initialize(CAM_PERSPECTIVE,1.0f,fZMax,(float)rect.Width()/2,(float)rect.Height()/2,60,1.0f);
			
	m_pCamera->SetTarget(vTarget);
	m_pCamera->SetPosition(Core::Vector3(vTarget.x,vTarget.y,vTarget.z - fMaxSize*1.5f));

	if(m_iMultiAction==3 && iSelected >= 1)
		m_dwLODDist=m_LODDistances[iSelected-1];
	else
		m_dwLODDist=0;

	UpdateData(FALSE);

	this->SetTimer(1,50,0);
}

void CMeshEditDlg::OnBnClickedMeditMultiselectrd()
{
	UpdateData(TRUE);
	GetDlgItem(IDC_MEDIT_LODDISTEB)->EnableWindow(FALSE);
	GetDlgItem(IDC_MEDIT_ANIMCK)->EnableWindow(TRUE);
	GetDlgItem(IDC_MEDIT_LODCK)->EnableWindow(m_bImportLOD ? FALSE : TRUE);
	GetDlgItem(IDC_MEDIT_CENTERCK)->EnableWindow(TRUE);

	m_bCenter=TRUE;
	UpdateData(FALSE);
}

void CMeshEditDlg::OnBnClickedMeditMultiselectrd2()
{
	if(m_bInited)
	{
		UpdateData(TRUE);
		m_iMultiAction=2;

		GetDlgItem(IDC_MEDIT_LODDISTEB)->EnableWindow(FALSE);
		GetDlgItem(IDC_MEDIT_ANIMCK)->EnableWindow(FALSE);
		GetDlgItem(IDC_MEDIT_LODCK)->EnableWindow(m_bImportLOD ? FALSE : TRUE);
		GetDlgItem(IDC_MEDIT_CENTERCK)->EnableWindow(FALSE);

		m_bCenter=FALSE;
		m_bImportAnim=FALSE;
		UpdateData(FALSE);
	}
}

void CMeshEditDlg::OnBnClickedMeditMultiselectrd3()
{
	if(m_bInited)
	{
		UpdateData(TRUE);
		m_iMultiAction=3;

		GetDlgItem(IDC_MEDIT_LODDISTEB)->EnableWindow(TRUE);
		GetDlgItem(IDC_MEDIT_ANIMCK)->EnableWindow(TRUE);
		GetDlgItem(IDC_MEDIT_LODCK)->EnableWindow(FALSE);
		GetDlgItem(IDC_MEDIT_CENTERCK)->EnableWindow(FALSE);

		m_bCenter=FALSE;
		m_bLODManage=FALSE;
		UpdateData(FALSE);
	}
}

void CMeshEditDlg::OnEnChangeMeditLoddisteb()
{
	if(!m_bInited) return;

	UpdateData(TRUE);

	if(m_iMultiAction==3)
	{
		const int iSelected=m_MeshCb.GetCurSel();
		if(iSelected > 0 && iSelected <= m_pLoader->GetMeshCount())
			m_LODDistances[iSelected-1]=m_dwLODDist;
	}
}

void CMeshEditDlg::OnCbnSelchangeMeditMatselectcb()
{
	// TODO : ajoutez ici le code de votre gestionnaire de notification de contrôle
}

void CMeshEditDlg::OnBnClickedMeditEditmatbtn()
{
	// TODO : ajoutez ici le code de votre gestionnaire de notification de contrôle
}

void CMeshEditDlg::OnBnClickedOk()
{
	KillTimer(1);

	if(!m_bImportLOD)
	{
		Core::HMesh hMesh=g_MeshMgr->GetMeshByName(m_strName);
		if(!hMesh.IsNull())
		{
			MessageBox("Un objet porte déjà ce nom.","Erreur",MB_OK | MB_ICONERROR);
			return;
		}
	}

	if(m_iMultiAction==1)
		BuildMesh();
	else if(m_iMultiAction==2)
		CombineMesh();
	else if(m_iMultiAction==3)
		BuildLODs();

	Release();

	if(m_bLODManage)
	{
		IMesh *pMesh=g_MeshMgr->GetMesh(m_hMesh);
		if(pMesh)
		{
			CLODDlg dlg;
			dlg.SetMesh(pMesh,"");
			dlg.DoModal();
		}
	}

	OnOK();
}

void CMeshEditDlg::OnBnClickedCancel()
{
	KillTimer(1);
	Release();
	OnCancel();
}

void CMeshEditDlg::OnEnChangeMeditName()
{
	if(m_bInited)
		UpdateData(TRUE);
}

void CMeshEditDlg::OnPaint()
{
	CPaintDC dc(this);

	Draw();
}

void CMeshEditDlg::ConvertMaterial(IMaterial *pMat, LMaterial *mat)
{
	uint32 dwFlags=0;

	pMat->SetDiffuseColor(mat->m_cDiffuseColor);

	if(mat->m_cEmissiveColor.r > 0 ||
	   mat->m_cEmissiveColor.g > 0 ||
	   mat->m_cEmissiveColor.b > 0)
	{
		pMat->SetEmissiveColor(mat->m_cEmissiveColor);
	   dwFlags|=SMAP_EMISSIVE;
	}

	if(mat->m_fShininess!=0.0f)
	{
		pMat->SetShininess(mat->m_fShininess);
		pMat->SetSpecularColor(mat->m_cSpecularColor);
		dwFlags|=SMAP_SPECULAR;
	}

	if(mat->m_bAlphaEnable)
	{
		if(strcmp(mat->m_szSrcBlend,"one")==0)
			pMat->SetSrcBlend(D3DBLEND_ONE);
		else if(strcmp(mat->m_szSrcBlend,"zero")==0)
			pMat->SetSrcBlend(D3DBLEND_ZERO);
		else if(strcmp(mat->m_szSrcBlend,"src_alpha")==0)
			pMat->SetSrcBlend(D3DBLEND_SRCALPHA);
		else if(strcmp(mat->m_szSrcBlend,"one_minus_src_alpha")==0)
			pMat->SetSrcBlend(D3DBLEND_INVSRCALPHA);
		else if(strcmp(mat->m_szSrcBlend,"dest_alpha")==0)
			pMat->SetSrcBlend(D3DBLEND_DESTALPHA);
		else if(strcmp(mat->m_szSrcBlend,"one_minue_dest_alpha")==0)
			pMat->SetSrcBlend(D3DBLEND_INVDESTALPHA);
		else if(strcmp(mat->m_szSrcBlend,"src_color")==0)
			pMat->SetSrcBlend(D3DBLEND_SRCCOLOR);
		else if(strcmp(mat->m_szSrcBlend,"one_minus_src_solor")==0)
			pMat->SetSrcBlend(D3DBLEND_INVSRCCOLOR);
		else if(strcmp(mat->m_szSrcBlend,"dest_color")==0)
			pMat->SetSrcBlend(D3DBLEND_DESTCOLOR);
		else if(strcmp(mat->m_szSrcBlend,"one_minus_dest_color")==0)
			pMat->SetSrcBlend(D3DBLEND_INVDESTCOLOR);
		else
			pMat->SetSrcBlend(D3DBLEND_SRCALPHA);

		if(strcmp(mat->m_szDestBlend,"one")==0)
			pMat->SetDstBlend(D3DBLEND_ONE);
		else if(strcmp(mat->m_szDestBlend,"zero")==0)
			pMat->SetDstBlend(D3DBLEND_ZERO);
		else if(strcmp(mat->m_szDestBlend,"src_alpha")==0)
			pMat->SetDstBlend(D3DBLEND_SRCALPHA);
		else if(strcmp(mat->m_szDestBlend,"one_minus_src_alpha")==0)
			pMat->SetDstBlend(D3DBLEND_INVSRCALPHA);
		else if(strcmp(mat->m_szDestBlend,"dest_alpha")==0)
			pMat->SetDstBlend(D3DBLEND_DESTALPHA);
		else if(strcmp(mat->m_szDestBlend,"one_minue_dest_alpha")==0)
			pMat->SetDstBlend(D3DBLEND_INVDESTALPHA);
		else if(strcmp(mat->m_szDestBlend,"src_color")==0)
			pMat->SetDstBlend(D3DBLEND_SRCCOLOR);
		else if(strcmp(mat->m_szDestBlend,"one_minus_src_solor")==0)
			pMat->SetDstBlend(D3DBLEND_INVSRCCOLOR);
		else if(strcmp(mat->m_szDestBlend,"dest_color")==0)
			pMat->SetDstBlend(D3DBLEND_DESTCOLOR);
		else if(strcmp(mat->m_szDestBlend,"one_minus_dest_color")==0)
			pMat->SetDstBlend(D3DBLEND_INVDESTCOLOR);
		else
			pMat->SetDstBlend(D3DBLEND_SRCALPHA);
	}

	if(strcmp(mat->m_szDepthFunction,"never")==0)
		pMat->SetDepthFnc(D3DCMP_NEVER);
	else if(strcmp(mat->m_szDepthFunction,"always")==0)
		pMat->SetDepthFnc(D3DCMP_ALWAYS);
	else if(strcmp(mat->m_szDepthFunction,"less")==0)
		pMat->SetDepthFnc(D3DCMP_LESS);
	else if(strcmp(mat->m_szDepthFunction,"lequal")==0)
		pMat->SetDepthFnc(D3DCMP_LESSEQUAL);
	else if(strcmp(mat->m_szDepthFunction,"equal")==0)
		pMat->SetDepthFnc(D3DCMP_EQUAL);
	else if(strcmp(mat->m_szDepthFunction,"nequal")==0)
		pMat->SetDepthFnc(D3DCMP_NOTEQUAL);
	else if(strcmp(mat->m_szDepthFunction,"gequal")==0)
		pMat->SetDepthFnc(D3DCMP_GREATEREQUAL);
	else if(strcmp(mat->m_szDepthFunction,"greater")==0)
		pMat->SetDepthFnc(D3DCMP_GREATER);
	else
		pMat->SetDepthFnc(D3DCMP_LESSEQUAL);

	if(strcmp(mat->m_szStencilFunction,"never")==0)
		pMat->SetStencilFnc(D3DCMP_NEVER);
	else if(strcmp(mat->m_szStencilFunction,"always")==0)
		pMat->SetStencilFnc(D3DCMP_ALWAYS);
	else if(strcmp(mat->m_szStencilFunction,"less")==0)
		pMat->SetStencilFnc(D3DCMP_LESS);
	else if(strcmp(mat->m_szStencilFunction,"lequal")==0)
		pMat->SetStencilFnc(D3DCMP_LESSEQUAL);
	else if(strcmp(mat->m_szStencilFunction,"equal")==0)
		pMat->SetStencilFnc(D3DCMP_EQUAL);
	else if(strcmp(mat->m_szStencilFunction,"nequal")==0)
		pMat->SetStencilFnc(D3DCMP_NOTEQUAL);
	else if(strcmp(mat->m_szStencilFunction,"gequal")==0)
		pMat->SetStencilFnc(D3DCMP_GREATEREQUAL);
	else if(strcmp(mat->m_szStencilFunction,"greater")==0)
		pMat->SetStencilFnc(D3DCMP_GREATER);
	else
		pMat->SetStencilFnc(D3DCMP_LESSEQUAL);

	pMat->SetZWriteEnable(mat->m_bZWriteEnable ? TRUE : FALSE);
	
	if(strcmp(mat->m_szCullMode,"none")==0)
		pMat->SetCullMode(D3DCULL_NONE);
	else if(strcmp(mat->m_szCullMode,"front")==0)
		pMat->SetCullMode(D3DCULL_CW);
	else
		pMat->SetCullMode(D3DCULL_CCW);

	pMat->SetCastShadows(mat->m_bCastShadows);
	pMat->SetReceiveShadows(mat->m_bReceiveShadows);

	int iCurrentStage=0;
	if(strlen(mat->m_szBaseTexture)!=0)
	{
		Core::HTexture hTex=g_TextureMgr->LoadTexture(mat->m_szBaseTexture,m_strPath);
		if(hTex.IsNull())
			MessageBox("Erreur au chargement de la texture de base.","Erreur",MB_OK | MB_ICONERROR);
		else
		{
			pMat->AddTexture(hTex,iCurrentStage);
			dwFlags|=SMAP_TEXBASE;
			iCurrentStage++;
		}
	}

	if(strlen(mat->m_szLightTexture)!=0)
	{
		Core::HTexture hTex=g_TextureMgr->LoadTexture(mat->m_szLightTexture,m_strPath);
		if(hTex.IsNull())
			MessageBox("Erreur au chargement de la texture de lumière.","Erreur",MB_OK | MB_ICONERROR);
		else
		{
			pMat->AddTexture(hTex,iCurrentStage);
			if(iCurrentStage==0)
				dwFlags|=SMAP_TEXBASE;
			else
				dwFlags|=SMAP_TEXSECFILTER;

			iCurrentStage++;
		}
	}

	if(strlen(mat->m_szBumpTexture)!=0)
	{
		Core::HTexture hTex=g_TextureMgr->LoadTexture(mat->m_szBumpTexture,m_strPath);
		if(hTex.IsNull())
			MessageBox("Erreur au chargement de la texture de bump.","Erreur",MB_OK | MB_ICONERROR);
		else
		{
			pMat->AddTexture(hTex,iCurrentStage);
			dwFlags|=SMAP_BUMP;
			iCurrentStage++;
		}
	}

	if(strlen(mat->m_szEnvironmentTexture)!=0)
	{
		Core::HTexture hTex=g_TextureMgr->LoadTexture(mat->m_szEnvironmentTexture,m_strPath);
		if(hTex.IsNull())
			MessageBox("Erreur au chargement de la texture de base.","Erreur",MB_OK | MB_ICONERROR);
		else
		{
			pMat->AddTexture(hTex,iCurrentStage);
			dwFlags|=SMAP_ENVIRONMENT;
			iCurrentStage++;
		}
	}

	if(strlen(mat->m_szShaderName)!=0)
		pMat->SetShader(RENDERINGPASS_SPECIAL,g_ShaderMgr->LoadShader(mat->m_szShaderName));

	char szAmbientShader[32];
	char szSunShader[32];

	if(dwFlags & SMAP_TEXBASE)
	{
		if(pMat->IsEmissiveEnable())
			strcpy(szAmbientShader,"StdAmbient_EmissTex");
		else
			strcpy(szAmbientShader,"StdAmbient_Tex");

		strcpy(szSunShader,"StdSun_Tex");
	}
	else
	{
		if(pMat->IsEmissiveEnable())
			strcpy(szAmbientShader,"StdAmbient_EmissNoTex");
		else
			strcpy(szAmbientShader,"StdAmbient_NoTex");

		strcpy(szSunShader,"StdSun_NoTex");
	}

	pMat->SetShader(RENDERINGPASS_AMBIENT,g_ShaderMgr->LoadShader(szAmbientShader));
	pMat->SetShader(RENDERINGPASS_SUN,g_ShaderMgr->LoadShader(szSunShader));
}

void CMeshEditDlg::Draw()
{
	g_Renderer->SetView(m_Preview.m_hWnd);
	g_Renderer->BeginScene();

	g_Renderer->SetVertexDeclaration(Core::VX_PNDTT);

	g_Renderer->SetTransformation(TRANS_VIEWPROJ,m_pCamera->GetViewProjMatrix());
	g_Renderer->SetTransformation(TRANS_MODEL,Core::Matrix4::Identity,0,true);

	const int iSelected=m_MeshCb.GetCurSel();
	if(iSelected==0)
	{
		for(int i=0;i<m_pLoader->GetMeshCount();i++)
		{
			Core::VertexPNDTT *pVert;
			int iVertCount=m_pLoader->GetVertices(i,&pVert);

			uint16 *pInd;
			int iTriCount=m_pLoader->GetIndices(i,&pInd);

			for(int j=0;j<m_pLoader->GetSubMeshCount(i);j++)
			{
				int iOffset,iCount,iMatID;
				m_pLoader->GetSubMesh(i,j,iOffset,iCount,iMatID);

				if(iMatID >= 0 && iMatID < m_Materials.size())
					m_Materials[iMatID]->Set(0);
				else
					m_Materials[0]->Set(0);

				g_Renderer->RenderDynamicBuffers(D3DPT_TRIANGLELIST,iVertCount,pVert,Core::VX_PNDTT,iCount*3,pInd+iOffset*3,D3DFMT_INDEX16);
			}
		}
	}
	else if(iSelected!=-1)
	{
		Core::VertexPNDTT *pVert;
		int iVertCount=m_pLoader->GetVertices(iSelected-1,&pVert);

		uint16 *pInd;
		int iTriCount=m_pLoader->GetIndices(iSelected-1,&pInd);

		for(int j=0;j<m_pLoader->GetSubMeshCount(iSelected-1);j++)
		{
			int iOffset,iCount,iMatID;
			m_pLoader->GetSubMesh(iSelected-1,j,iOffset,iCount,iMatID);

			if(iMatID >= 0 && iMatID < m_Materials.size())
				m_Materials[iMatID]->Set(0);
			else
				m_Materials[0]->Set(0);

			g_Renderer->RenderDynamicBuffers(D3DPT_TRIANGLELIST,iVertCount,pVert,Core::VX_PNDTT,iCount*3,pInd+iOffset*3,D3DFMT_INDEX16);
		}
	}

	g_Renderer->EndScene();
}

void CMeshEditDlg::Release()
{
	for(int i=0;i<m_Materials.size();i++)
	{
		Core::HMaterial hMat=g_MaterialMgr->GetMaterialByName(m_Materials[i]->GetName());
		if(hMat.IsNull())
			g_MaterialMgr->ReleaseMaterialWithoutRegistering(m_Materials[i]);
	}

	m_Materials.clear();
	m_pLoader->Release();
}

void CMeshEditDlg::BuildMesh()
{
	int iSelected=m_MeshCb.GetCurSel();
	if(iSelected < 1) iSelected=1;

	if(m_bCenter)
	{
		Core::BBox bbox(m_pLoader->GetMeshBBox(iSelected-1));
		const Core::Vector3 vCenter((bbox.m_vMin + bbox.m_vMax) * 0.5f);
		Core::VertexPNDTT *pVert;
		int iVCount=m_pLoader->GetVertices(iSelected-1,&pVert);

		for(int j=0;j<iVCount;j++)
			pVert[j].vPos-=vCenter;
	}

	if(m_bImportLOD)
		ConstructMesh(iSelected-1,m_pMesh);
	else
	{
		m_hMesh=g_MeshMgr->CreateEmptyMesh(m_strName);
		IMesh *pMesh=g_MeshMgr->GetMesh(m_hMesh);
		ConstructMesh(iSelected-1,pMesh);
	}
}

void CMeshEditDlg::CombineMesh()
{
	Core::TDynamicTab<Core::VertexPNDTT> verts;
	Core::TDynamicTab<ISubMesh *> subs;

	int iVOffset=0;
	int iIOffset=0;
	int iTotalTri=0;

	for(int i=0;i<m_pLoader->GetMeshCount();i++)
	{
		uint16 *pTemp;
		iTotalTri+=m_pLoader->GetIndices(i,&pTemp);
	}

	uint16 *pIndices=new uint16[iTotalTri*3];

	Core::HMaterial hMat;
	IMaterial *pMat;

	for(int i=0;i<m_pLoader->GetMeshCount();i++)
	{
		Core::VertexPNDTT *pVerts;
		uint16 *pInd;

		int iVCount=m_pLoader->GetVertices(i,&pVerts);
		int iTriCount=m_pLoader->GetIndices(i,&pInd);

		uint16 *pTemp=pIndices+iIOffset;
		for(int j=0;j<iTriCount*3;j++)
			pTemp[j]=pInd[j]+iVOffset;

		verts.AddTab(pVerts,iVCount);

		for(int j=0;j<m_pLoader->GetSubMeshCount(i);j++)
		{
			ISubMesh *pSub=g_MeshMgr->CreateNewSubMesh();
			int iTriOffset, iCount, iMatID;
			m_pLoader->GetSubMesh(i,j,iTriOffset,iCount,iMatID);

			if(iMatID >= 0 && iMatID < m_Materials.size())
				pMat=m_Materials[iMatID];
			else
				pMat=m_Materials[0];

			hMat=g_MaterialMgr->GetMaterialByName(pMat->GetName());
			if(hMat.IsNull())
				hMat=g_MaterialMgr->RegisterMaterial(pMat);

			pSub->Construct(iIOffset/3 + iTriOffset,iCount,hMat);
			subs.Add(pSub);
		}

		iIOffset+=iTriCount*3;
		iVOffset+=iVCount;
	}

	if(subs.GetItemCount()==0)
		return;

	IMesh *pMesh;

	if(m_bImportLOD)
		pMesh=m_pMesh;
	else
	{
		m_hMesh=g_MeshMgr->CreateEmptyMesh(m_strName);
		pMesh=g_MeshMgr->GetMesh(m_hMesh);
	}

	pMesh->Construct(m_strName,verts.GetItemCount(),verts.GetBuffer(),
		iTotalTri, pIndices,subs.GetBuffer()[0],0);

	delete[] pIndices;

	for(int i=1;i<subs.GetItemCount();i++)
		pMesh->AddSubMesh(subs.GetBuffer()[i]);
}

void CMeshEditDlg::BuildLODs()
{
	const int iMeshCount=MIN(m_pLoader->GetMeshCount(),MAX_MESH_COUNT);
	if(iMeshCount==0) return;

	int index[MAX_MESH_COUNT];
	bool bInsert[MAX_MESH_COUNT];

	memset(bInsert,0,MAX_MESH_COUNT*sizeof(bool));

	//			Tri pas tres rapide mais le nombre d'items est faible
	uint32 current_lod=0;
	for(int i=0;i<iMeshCount;i++)
	{
		int min_index=-1;
		uint32 min_lod=0xffffffff;
		for(int j=0;j<iMeshCount;j++)
		{
			if(!bInsert[j])
			{
				const uint32 dwDist=m_LODDistances[j];
				if(dwDist >= current_lod && dwDist <= min_lod)
				{
					min_index=j;
					min_lod=dwDist;
				}
			}
		}

		index[i]=min_index;
		current_lod=min_lod;
		bInsert[min_index]=true;

		Core::BBox bbox(m_pLoader->GetMeshBBox(i));
		const Core::Vector3 vCenter((bbox.m_vMin + bbox.m_vMax) * 0.5f);
		Core::VertexPNDTT *pVert;
		int iVCount=m_pLoader->GetVertices(i,&pVert);

		for(int j=0;j<iVCount;j++)
			pVert[j].vPos-=vCenter;
	}

	m_hMesh=g_MeshMgr->CreateEmptyMesh(m_strName);
	IMesh *pMesh=g_MeshMgr->GetMesh(m_hMesh);

	ConstructMesh(index[0],pMesh,m_LODDistances[index[0]]);
	for(int i=1;i<iMeshCount;i++)
	{
		IMesh *pLOD=pMesh->CreateLOD(m_LODDistances[index[i]]);
		ConstructMesh(index[i],pLOD,m_LODDistances[index[i]]);
	}
}

void CMeshEditDlg::ConstructMesh(int iIndex, IMesh *pMesh, uint32 dwLODDist)
{
	ISubMesh *pSub=g_MeshMgr->CreateNewSubMesh();

	int iOffset,iCount,iMatID;
	m_pLoader->GetSubMesh(iIndex,0,iOffset,iCount,iMatID);

	IMaterial *pMat;
	if(iMatID >= 0 && iMatID < m_Materials.size())
		pMat=m_Materials[iMatID];
	else
		pMat=m_Materials[0];

	Core::HMaterial hMat=g_MaterialMgr->GetMaterialByName(pMat->GetName());
	if(hMat.IsNull())
		hMat=g_MaterialMgr->RegisterMaterial(pMat);

	Core::VertexPNDTT *pVert;
	int iVertCount=m_pLoader->GetVertices(iIndex,&pVert);

	uint16 *pInd;
	int iTriCount=m_pLoader->GetIndices(iIndex,&pInd);

	pSub->Construct(iOffset,iCount,hMat);
	pMesh->Construct(m_strName,iVertCount,pVert,iTriCount,pInd,pSub,dwLODDist*dwLODDist);

	if(m_pLoader->GetSubMeshCount(iIndex) > 1)
	{
		for(int i=1;i<m_pLoader->GetSubMeshCount(iIndex);i++)
		{
			pSub=g_MeshMgr->CreateNewSubMesh();

			m_pLoader->GetSubMesh(iIndex,i,iOffset,iCount,iMatID);

			if(iMatID >= 0 && iMatID < m_Materials.size())
				pMat=m_Materials[iMatID];
			else
				pMat=m_Materials[0];

			hMat=g_MaterialMgr->GetMaterialByName(pMat->GetName());
			if(hMat.IsNull())
				hMat=g_MaterialMgr->RegisterMaterial(pMat);

			pSub->Construct(iOffset,iCount,hMat);
			pMesh->AddSubMesh(pSub);
		}
	}
}
void CMeshEditDlg::OnBnClickedMeditAnimck()
{
	UpdateData(TRUE);
}

void CMeshEditDlg::OnBnClickedMeditLodck()
{
	UpdateData(TRUE);
}

void CMeshEditDlg::OnBnClickedMeditImportmatbtn()
{
}

void CMeshEditDlg::OnBnClickedMeditCenterck()
{
	UpdateData(TRUE);
}
