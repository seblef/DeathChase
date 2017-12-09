// EnvironmentDlg.cpp : fichier d'implémentation
//

#include "stdafx.h"
#include "DeathEditor.h"
#include "DeathEditorDoc.h"
#include "EnvironmentDlg.h"

#include "WindowMgr.h"
#include "FileSystem.h"
#include "Interfaces.h"
#include "../DeathEngine/Export/IRenderer.h"
#include "../DeathEngine/Export/IRenderer2D.h"
#include "../DeathEngine/Export/ITextureMgr.h"
#include "../DeathEngine/Export/IMap.h"
#include "../DeathEngine/Export/ISkyBox.h"
#include "../DeathEngine/Export/IShield.h"
#include "../DeathEngine/Export/IHeightmap.h"
#include "../DeathEngine/Export/ISun.h"
#include "../DeathEngine/Export/IObjectMap.h"
#include "../DeathEngine/Export/IWater.h"

#define WEATHERTYPE_SUN				0
#define WEATHERTYPE_CLOUD			1
#define WEATHERTYPE_RAIN			2
#define WEATHERTYPE_SNOW			3

#define ENVDRAW_NONE				-1
#define ENVDRAW_SKYBOX				0
#define ENVDRAW_WEATHER				1
#define ENVDRAW_SUN					2
#define ENVDRAW_SHIELD				3

// Boîte de dialogue CEnvironmentDlg

IMPLEMENT_DYNAMIC(CEnvironmentDlg, CDialog)

CEnvironmentDlg::CEnvironmentDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEnvironmentDlg::IDD, pParent)
	, m_iWeatherType(0)
	, m_iWeatherDensity(128)
	, m_bGenerateWater(FALSE)
	, m_bGenerateTrees(FALSE),
	m_bMinimized(false),
	m_bInited(false),
	m_iCurrentDraw(ENVDRAW_NONE)
	, m_dwShieldDist(0)
{

}

CEnvironmentDlg::~CEnvironmentDlg()
{
}

void CEnvironmentDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ENV_SKYLIST, m_SkyList);
	DDX_CBIndex(pDX, IDC_ENV_WEATHER_TYPECB, m_iWeatherType);
	DDX_Text(pDX, IDC_ENV_WEATHER_DENSITYEB, m_iWeatherDensity);
	DDX_Check(pDX, IDC_ENV_GENERATEWATERCK, m_bGenerateWater);
	DDX_Check(pDX, IDC_ENV_GENERATETREESCK, m_bGenerateTrees);
	DDX_Control(pDX, IDC_ENV_PREVIEW, m_Preview);
	DDX_Control(pDX, IDC_ENV_WEATHER_DENSITYSB, m_WeatherDensitySb);
	DDX_Text(pDX, IDC_ENV_SHIELD_DISTEB, m_dwShieldDist);
	DDX_Control(pDX, IDC_ENV_SHIELD_DISTSB, m_ShieldSb);
	DDX_Control(pDX, IDC_ENV_AMBIENTBTN, m_AmbientBtn);
	DDX_Control(pDX, IDC_ENV_SUN_COLORBTN, m_SunColorBtn);
}


BEGIN_MESSAGE_MAP(CEnvironmentDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_SHOWWINDOW()
	ON_WM_SIZE()
	ON_LBN_SELCHANGE(IDC_ENV_SKYLIST, &CEnvironmentDlg::OnLbnSelchangeEnvSkylist)
	ON_BN_CLICKED(IDC_ENV_SKYTEXBTN, &CEnvironmentDlg::OnBnClickedEnvSkytexbtn)
	ON_CBN_SELCHANGE(IDC_ENV_WEATHER_TYPECB, &CEnvironmentDlg::OnCbnSelchangeEnvWeatherTypecb)
	ON_EN_CHANGE(IDC_ENV_WEATHER_DENSITYEB, &CEnvironmentDlg::OnEnChangeEnvWeatherDensityeb)
	ON_BN_CLICKED(IDC_ENV_WEATHER_TEXBTN, &CEnvironmentDlg::OnBnClickedEnvWeatherTexbtn)
	ON_BN_CLICKED(IDC_ENV_SUN_TEXBTN, &CEnvironmentDlg::OnBnClickedEnvSunTexbtn)
	ON_BN_CLICKED(IDC_ENV_SUN_COLORBTN, &CEnvironmentDlg::OnBnClickedEnvSunColorbtn)
	ON_BN_CLICKED(IDC_ENV_GENERATEWATERCK, &CEnvironmentDlg::OnBnClickedEnvGeneratewaterck)
	ON_BN_CLICKED(IDC_ENV_GENERATETREESCK, &CEnvironmentDlg::OnBnClickedEnvGeneratetreesck)
	ON_BN_CLICKED(IDC_ENV_PREVBTN, &CEnvironmentDlg::OnBnClickedEnvPrevbtn)
	ON_BN_CLICKED(IDC_ENV_NEXTBTN, &CEnvironmentDlg::OnBnClickedEnvNextbtn)
	ON_BN_CLICKED(IDC_ENV_SHIELD_TEXBTN, &CEnvironmentDlg::OnBnClickedEnvShieldTexbtn)
	ON_EN_CHANGE(IDC_ENV_SHIELD_DISTEB, &CEnvironmentDlg::OnEnChangeEnvShieldDisteb)
	ON_BN_CLICKED(IDC_ENV_AMBIENTBTN, &CEnvironmentDlg::OnBnClickedEnvAmbientbtn)
END_MESSAGE_MAP()


// Gestionnaires de messages de CEnvironmentDlg

BOOL CEnvironmentDlg::OnInitDialog()
{
	if(!CDialog::OnInitDialog())
		return FALSE;

	GetDlgItem(IDC_ENV_WEATHER_DENSITYEB)->EnableWindow(FALSE);
	GetDlgItem(IDC_ENV_WEATHER_TEXBTN)->EnableWindow(FALSE);
	m_WeatherDensitySb.EnableWindow(FALSE);

	m_SkyList.AddString("Dessus");
	m_SkyList.AddString("Face");
	m_SkyList.AddString("Droite");
	m_SkyList.AddString("Arrière");
	m_SkyList.AddString("Gauche");

	m_WeatherDensitySb.SetRange(1,255);
	m_ShieldSb.SetRange(0,4096);

	m_bInited=true;
	Reinit();

	return TRUE;
}

void CEnvironmentDlg::Reinit()
{
	m_bGenerateWater=FALSE;
	m_bGenerateTrees=FALSE;

	m_dwShieldDist=g_Map->GetHeightmap()->GetSize()/2;

	UpdateData(FALSE);

	GetDlgItem(IDC_ENV_SKYTEXBTN)->SetWindowText("<Texture>");
	GetDlgItem(IDC_ENV_SKYTEXBTN)->EnableWindow(FALSE);
	m_SkyList.SetCurSel(-1);

	GetDlgItem(IDC_ENV_SHIELD_TEXBTN)->SetWindowText("<Texture>");
	GetDlgItem(IDC_ENV_SUN_TEXBTN)->SetWindowText("<Texture>");

	Core::Color32f cColor(g_Map->GetSun()->GetColor());
	COLORREF c=RGB((int)(cColor.fr * 255.0f),(int)(cColor.fg * 255.0f),(int)(cColor.fb * 255.0f));

	m_SunColorBtn.SetColor(CButtonST::BTNST_COLOR_BK_IN,c);
	m_SunColorBtn.SetColor(CButtonST::BTNST_COLOR_BK_OUT,c);
	m_SunColorBtn.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS,c);

	const Core::Color32f cAmbient(g_Map->GetSun()->GetAmbientLight());
	c=RGB((int)(cAmbient.fr * 255.0f),(int)(cAmbient.fg * 255.0f),(int)(cAmbient.fb * 255.0f));

	m_AmbientBtn.SetColor(CButtonST::BTNST_COLOR_BK_IN,c);
	m_AmbientBtn.SetColor(CButtonST::BTNST_COLOR_BK_OUT,c);
	m_AmbientBtn.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS,c);
}

void CEnvironmentDlg::OnOK()
{
}

void CEnvironmentDlg::OnCancel()
{
}


void CEnvironmentDlg::OnPaint()
{
	CPaintDC dc(this);

	if(!m_bMinimized) Draw();
}

void CEnvironmentDlg::OnShowWindow(BOOL bShow, UINT nStatus)
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

		const float fSize=((float)g_Map->GetHeightmap()->GetSize())*0.5f;
		float fScale=g_Map->GetHeightmap()->GetScale();
		if(fScale==0.0f) fScale=255.0f;

		const Core::Vector3 vShieldSize(fSize,fScale*1.5f,fSize);

		g_Map->GetShield()->SetSize(vShieldSize);
		g_Map->GetShield()->EnableShield(true);
	}
	else
	{
		g_Map->GetShield()->EnableShield(false);
		g_Renderer->RemoveView(m_Preview.m_hWnd);
	}
}

void CEnvironmentDlg::OnSize(UINT nType, int cx, int cy)
{
	m_bMinimized=cx < 50 || cy < 50;
	CDialog::OnSize(nType, cx, cy);
}

void CEnvironmentDlg::OnLbnSelchangeEnvSkylist()
{
	const int iSelected=m_SkyList.GetCurSel();
	GetDlgItem(IDC_ENV_SKYTEXBTN)->EnableWindow(iSelected==-1 ? FALSE : TRUE);

	Core::HTexture hTex=g_Map->GetSkyBox()->GetTexture((SkyFace)iSelected);
	if(!hTex.IsNull())
	{
		char szTemp[128];
		g_TextureMgr->GetTextureName(hTex,szTemp);
		GetDlgItem(IDC_ENV_SKYTEXBTN)->SetWindowText(szTemp);
	}
	else
		GetDlgItem(IDC_ENV_SKYTEXBTN)->SetWindowText("<Texture>");

	m_iCurrentDraw=ENVDRAW_SKYBOX;
}

void CEnvironmentDlg::OnBnClickedEnvSkytexbtn()
{
	g_FileSystem.ChangeWorkingDirectory(DIR_SKYBOXTEXTURES);
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

	g_Map->GetSkyBox()->SetTexture((SkyFace)m_SkyList.GetCurSel(),hTex);
	GetDlgItem(IDC_ENV_SKYTEXBTN)->SetWindowText(dialog.GetFileName());

	m_iCurrentDraw=ENVDRAW_SKYBOX;
	Draw();
	g_WindowMgr.GetDoc()->UpdateAllViews(0);
}

void CEnvironmentDlg::OnCbnSelchangeEnvWeatherTypecb()
{
	UpdateData(TRUE);
	const BOOL bParticles=(m_iWeatherType==WEATHERTYPE_RAIN || m_iWeatherType==WEATHERTYPE_SNOW);

	GetDlgItem(IDC_ENV_WEATHER_DENSITYEB)->EnableWindow(bParticles);
	GetDlgItem(IDC_ENV_WEATHER_TEXBTN)->EnableWindow(bParticles);
	m_WeatherDensitySb.EnableWindow(bParticles);
}

void CEnvironmentDlg::OnEnChangeEnvWeatherDensityeb()
{
	if(m_bInited) UpdateData(TRUE);
}

void CEnvironmentDlg::OnBnClickedEnvWeatherTexbtn()
{
	// TODO : ajoutez ici le code de votre gestionnaire de notification de contrôle
}

void CEnvironmentDlg::OnBnClickedEnvSunTexbtn()
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

	g_Map->GetSun()->SetTexture(hTex);
	GetDlgItem(IDC_ENV_SUN_TEXBTN)->SetWindowText(dialog.GetFileName());

	m_iCurrentDraw=ENVDRAW_SUN;
	Draw();
	g_WindowMgr.GetDoc()->UpdateAllViews(0);}

void CEnvironmentDlg::OnBnClickedEnvSunColorbtn()
{
	const Core::Color32f cColor(g_Map->GetSun()->GetColor());
	CColorDialog dlg(RGB((int)(cColor.fr * 255.0f),(int)(cColor.fg*255.0f),(int)(cColor.fb*255.0f)),0,this);

	if(dlg.DoModal()==IDCANCEL)
		return;

	COLORREF c=dlg.GetColor();
	g_Map->GetSun()->SetColor(Core::Color32f(1.0f,((float)GetRValue(c)) / 255.0f,
		((float)GetGValue(c)) / 255.0f, ((float)GetBValue(c)) / 255.0f));

	m_SunColorBtn.SetColor(CButtonST::BTNST_COLOR_BK_IN,c);
	m_SunColorBtn.SetColor(CButtonST::BTNST_COLOR_BK_OUT,c);
	m_SunColorBtn.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS,c);

	g_WindowMgr.GetDoc()->UpdateAllViews(0);
}


void CEnvironmentDlg::OnBnClickedEnvGeneratewaterck()
{
	UpdateData(TRUE);
	if(m_bGenerateWater || m_bGenerateTrees)
		GetDlgItem(IDC_ENV_NEXTBTN)->SetWindowText("Suivant >");
	else
		GetDlgItem(IDC_ENV_NEXTBTN)->SetWindowText("Terminer");
}

void CEnvironmentDlg::OnBnClickedEnvGeneratetreesck()
{
	UpdateData(TRUE);
	if(m_bGenerateWater || m_bGenerateTrees)
		GetDlgItem(IDC_ENV_NEXTBTN)->SetWindowText("Suivant >");
	else
		GetDlgItem(IDC_ENV_NEXTBTN)->SetWindowText("Terminer");
}

void CEnvironmentDlg::OnBnClickedEnvPrevbtn()
{
	g_WindowMgr.HideDialog(WT_ENVIRONMENT);
	g_WindowMgr.ShowDialog(WT_TEXTURE,false);
}

void CEnvironmentDlg::OnBnClickedEnvNextbtn()
{
	if(!m_bGenerateWater)
	{
		IObjectMap *pMap=g_Map->GetObjectMap();
		pMap->Generate(g_Map->GetHeightmap(),8,0.0f);
	}

	g_WindowMgr.HideDialog(WT_ENVIRONMENT);

	if(m_bGenerateWater)
		g_WindowMgr.ShowDialog(WT_WATER,false);
	else if(m_bGenerateTrees)
		g_WindowMgr.ShowDialog(WT_TREES,false);
	else
	{
		g_WindowMgr.ReinitAllDialogs();
		((CDeathEditorDoc *)g_WindowMgr.GetDoc())->EnableMenus(true);
		g_WindowMgr.GetDoc()->UpdateAllViews(0,UPDATE_MAPCREATED);
	}
}

void CEnvironmentDlg::Draw()
{
	if(m_iCurrentDraw==ENVDRAW_SKYBOX)
	{
		g_Renderer->SetView(m_Preview.m_hWnd);
		g_Renderer->BeginScene();

		g_2DRenderer->Start();

		CRect rect;
		m_Preview.GetClientRect(&rect);
		int dx=rect.Width() / 3;
		int dy=rect.Height() / 3;

		Core::HTexture hTex=g_Map->GetSkyBox()->GetTexture(SKY_BACK);
		if(!hTex.IsNull()) g_2DRenderer->Draw2DImage(hTex,dx,0,2*dx,dy);

		hTex=g_Map->GetSkyBox()->GetTexture(SKY_LEFT);
		if(!hTex.IsNull()) g_2DRenderer->Draw2DImage(hTex,0,dy,dx,dy*2);

		hTex=g_Map->GetSkyBox()->GetTexture(SKY_UP);
		if(!hTex.IsNull()) g_2DRenderer->Draw2DImage(hTex,dx,dy,2*dx,2*dy);

		hTex=g_Map->GetSkyBox()->GetTexture(SKY_RIGHT);
		if(!hTex.IsNull()) g_2DRenderer->Draw2DImage(hTex,dx*2,dy,dx*3,dy*2);

		hTex=g_Map->GetSkyBox()->GetTexture(SKY_FRONT);
		if(!hTex.IsNull()) g_2DRenderer->Draw2DImage(hTex,dx,dy*2,dx*2,dy*3);

		g_2DRenderer->End();
		g_Renderer->EndScene();
	}
	else if(m_iCurrentDraw==ENVDRAW_SHIELD)
		g_Renderer->DrawImageInView(g_Map->GetShield()->GetTexture(),m_Preview.m_hWnd);
	else if(m_iCurrentDraw==ENVDRAW_SUN)
		g_Renderer->DrawImageInView(g_Map->GetSun()->GetTexture(),m_Preview.m_hWnd);
}


void CEnvironmentDlg::OnBnClickedEnvShieldTexbtn()
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

	g_Map->GetShield()->SetTexture(hTex);
	GetDlgItem(IDC_ENV_SHIELD_TEXBTN)->SetWindowText(dialog.GetFileName());

	m_iCurrentDraw=ENVDRAW_SHIELD;
	Draw();
	g_WindowMgr.GetDoc()->UpdateAllViews(0);
}

void CEnvironmentDlg::OnEnChangeEnvShieldDisteb()
{
	if(m_bInited)
	{
		UpdateData(TRUE);
		g_Map->GetShield()->SetVisibilityDistance(m_dwShieldDist);
		g_WindowMgr.GetDoc()->UpdateAllViews(0);
	}
}

void CEnvironmentDlg::OnBnClickedEnvAmbientbtn()
{
	const Core::Color32f cColor(g_Map->GetSun()->GetAmbientLight());
	CColorDialog dlg(RGB((int)(cColor.fr * 255.0f),(int)(cColor.fg*255.0f),(int)(cColor.fb*255.0f)),0,this);

	if(dlg.DoModal()==IDCANCEL)
		return;

	COLORREF c=dlg.GetColor();
	g_Map->GetSun()->SetAmbientLight(Core::Color32f(1.0f,((float)GetRValue(c)) / 255.0f,
		((float)GetGValue(c)) / 255.0f, ((float)GetBValue(c)) / 255.0f));

	m_AmbientBtn.SetColor(CButtonST::BTNST_COLOR_BK_IN,c);
	m_AmbientBtn.SetColor(CButtonST::BTNST_COLOR_BK_OUT,c);
	m_AmbientBtn.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS,c);

	g_WindowMgr.GetDoc()->UpdateAllViews(0);
}
