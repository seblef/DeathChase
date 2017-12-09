// WaterDlg.cpp : fichier d'implémentation
//

#include "stdafx.h"
#include "DeathEditor.h"
#include "WaterDlg.h"

#include "DeathEditorDoc.h"
#include "EnvironmentDlg.h"

#include "WindowMgr.h"
#include "Interfaces.h"
#include "FileSystem.h"
#include "../DeathEngine/Export/IRenderer.h"
#include "../DeathEngine/Export/ITextureMgr.h"
#include "../DeathEngine/Export/IMap.h"
#include "../DeathEngine/Export/IHeightmap.h"
#include "../DeathEngine/Export/IWater.h"
#include "../DeathEngine/Export/IObjectMap.h"


// Boîte de dialogue CWaterDlg

IMPLEMENT_DYNAMIC(CWaterDlg, CDialog)

CWaterDlg::CWaterDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWaterDlg::IDD, pParent)
	, m_iHeight(10)
	, m_bSurround(FALSE),
	m_bInited(false),
	m_bMinimized(false)
{

}

CWaterDlg::~CWaterDlg()
{
}

void CWaterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_WATER_HEIGHTEB, m_iHeight);
	DDX_Check(pDX, IDC_WATER_SURROUNDCK, m_bSurround);
	DDX_Control(pDX, IDC_WATER_PREVIEW, m_Preview);
	DDX_Control(pDX, IDC_WATER_HEIGHTSB, m_HeightSb);
}


BEGIN_MESSAGE_MAP(CWaterDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_SHOWWINDOW()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_WATER_TEXTUREBTN, &CWaterDlg::OnBnClickedWaterTexturebtn)
	ON_EN_CHANGE(IDC_WATER_HEIGHTEB, &CWaterDlg::OnEnChangeWaterHeighteb)
	ON_BN_CLICKED(IDC_WATER_SURROUNDCK, &CWaterDlg::OnBnClickedWaterSurroundck)
	ON_BN_CLICKED(IDC_WATER_APPLYBTN, &CWaterDlg::OnBnClickedWaterApplybtn)
	ON_BN_CLICKED(IDC_WATER_PREV, &CWaterDlg::OnBnClickedWaterPrev)
	ON_BN_CLICKED(IDC_WATER_NEXT, &CWaterDlg::OnBnClickedWaterNext)
END_MESSAGE_MAP()


// Gestionnaires de messages de CWaterDlg

BOOL CWaterDlg::OnInitDialog()
{
	if(!CDialog::OnInitDialog())
		return FALSE;

	m_HeightSb.SetRange(0,254);
	m_bInited=true;

	Reinit();

	return TRUE;
}

void CWaterDlg::Reinit()
{
	m_iHeight=10;
	m_bSurround=false;
	m_hTexture=Core::HTexture();

	UpdateData(FALSE);
}

void CWaterDlg::OnOK()
{
}

void CWaterDlg::OnCancel()
{
}

void CWaterDlg::OnPaint()
{
	CPaintDC dc(this);

	if(!m_bMinimized) g_Renderer->DrawImageInView(m_hTexture,m_Preview.m_hWnd);
}

void CWaterDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	if(bShow)
	{
		CRect rect;
		m_Preview.GetClientRect(&rect);
		g_Renderer->AddView(m_Preview.m_hWnd,rect.Width(),rect.Height());
		g_Renderer->SetView(m_Preview.m_hWnd);
		g_Renderer->BeginScene();
		g_Renderer->EndScene();

		if(((CEnvironmentDlg *)g_WindowMgr.GetDialog(WT_ENVIRONMENT))->m_bGenerateTrees)
			GetDlgItem(IDC_WATER_NEXT)->SetWindowText("Suivant >");
		else
			GetDlgItem(IDC_WATER_NEXT)->SetWindowText("Terminer");
	}
	else
		g_Renderer->RemoveView(m_Preview.m_hWnd);
}

void CWaterDlg::OnSize(UINT nType, int cx, int cy)
{
	m_bMinimized=cx < 50 || cy < 50;
	CDialog::OnSize(nType, cx, cy);
}

void CWaterDlg::OnBnClickedWaterTexturebtn()
{
	g_FileSystem.ChangeWorkingDirectory(DIR_TEXTURES);
	CString strPath(g_FileSystem.GetWorkingDirectory());
	strPath+=IMAGE_EXTPATH;

	CFileDialog dialog(TRUE,"*.jpg",strPath,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,IMAGE_EXTENSIONS);
	if(dialog.DoModal()==IDCANCEL)
	{
		g_FileSystem.ResetWorkingDirectory();
		return;
	}

	Core::HTexture hTex=g_TextureMgr->LoadTexture(dialog.GetFileName(),"");
	g_FileSystem.ResetWorkingDirectory();

	if(hTex.IsNull())
	{
		MessageBox("Erreur lors du chargement de la texture.","Erreur",MB_OK | MB_ICONERROR);
		return;
	}

	m_hTexture=hTex;
	g_Map->GetWater()->SetWaterTexture(hTex);
	GetDlgItem(IDC_WATER_TEXTUREBTN)->SetWindowText(dialog.GetFileName());

	g_Renderer->DrawImageInView(m_hTexture,m_Preview.m_hWnd);
	g_WindowMgr.GetDoc()->UpdateAllViews(0);
}

void CWaterDlg::OnEnChangeWaterHeighteb()
{
	if(m_bInited) UpdateData(TRUE);
}

void CWaterDlg::OnBnClickedWaterSurroundck()
{
	UpdateData(TRUE);
}

void CWaterDlg::OnBnClickedWaterApplybtn()
{
	float fHeight=(float)m_iHeight;
	if(g_Map->GetHeightmap()->GetScale()!=0.0f)
		fHeight*=g_Map->GetHeightmap()->GetScale() / 255.0f;

	g_Map->EnableWater(true);
	g_Map->GetWater()->Generate(g_Map->GetHeightmap(),8,fHeight,m_bSurround ? true : false,false);

	g_WindowMgr.GetDoc()->UpdateAllViews(0);
}

void CWaterDlg::OnBnClickedWaterPrev()
{
	g_WindowMgr.HideDialog(WT_WATER);
	g_WindowMgr.ShowDialog(WT_ENVIRONMENT,false);
}

void CWaterDlg::OnBnClickedWaterNext()
{
	g_Map->GetObjectMap()->Generate(g_Map->GetHeightmap(),8,g_Map->IsWaterEnable() ? g_Map->GetWater()->GetHeight() : 0.0f);

	g_WindowMgr.HideDialog(WT_WATER);
	if(((CEnvironmentDlg *)g_WindowMgr.GetDialog(WT_ENVIRONMENT))->m_bGenerateTrees)
		g_WindowMgr.ShowDialog(WT_TREES,false);
	else
	{
		g_WindowMgr.ReinitAllDialogs();
		g_WindowMgr.GetDoc()->UpdateAllViews(0,UPDATE_MAPCREATED);
		((CDeathEditorDoc *)g_WindowMgr.GetDoc())->EnableMenus(true);
	}
}
