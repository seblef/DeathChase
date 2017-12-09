// HeightmapDlg.cpp : fichier d'implémentation
//

#include "stdafx.h"
#include "DeathEditor.h"
#include "HeightmapDlg.h"

#include "FileSystem.h"
#include "WindowMgr.h"
#include "Interfaces.h"
#include "Data/IHeightmapGenerator.h"
#include "../DeathEngine/Export/IRenderer.h"
#include "../DeathEngine/Export/IMap.h"
#include "../DeathEngine/Export/IHeightmap.h"
#include "../DeathEngine/Export/IPatchGrid.h"
#include "../DeathEngine/Export/ITextureMgr.h"

// Boîte de dialogue CHeightmapDlg

IMPLEMENT_DYNAMIC(CHeightmapDlg, CDialog)

CHeightmapDlg::CHeightmapDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CHeightmapDlg::IDD, pParent)
	, m_iGenType(0)
	, m_iScale(100)
	, m_bGenerate(FALSE),
	m_pCurrentGenerator(0),
	m_bMinimized(false)
{

}

CHeightmapDlg::~CHeightmapDlg()
{
}

void CHeightmapDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_CBIndex(pDX, IDC_HGT_GENTYPECB, m_iGenType);
	DDX_Control(pDX, IDC_HGT_PARAM1EB, m_ParamEb[0]);
	DDX_Control(pDX, IDC_HGT_PARAM2EB, m_ParamEb[1]);
	DDX_Control(pDX, IDC_HGT_PARAM3EB, m_ParamEb[2]);
	DDX_Control(pDX, IDC_HGT_PARAM4EB, m_ParamEb[3]);
	DDX_Control(pDX, IDC_HGT_PARAM5EB, m_ParamEb[4]);
	DDX_Control(pDX, IDC_HGT_PARAM1ST, m_ParamSt[0]);
	DDX_Control(pDX, IDC_HGT_PARAM2ST, m_ParamSt[1]);
	DDX_Control(pDX, IDC_HGT_PARAM3ST, m_ParamSt[2]);
	DDX_Control(pDX, IDC_HGT_PARAM4ST, m_ParamSt[3]);
	DDX_Control(pDX, IDC_HGT_PARAM5ST, m_ParamSt[4]);
	DDX_Control(pDX, IDC_HGT_PARAM1SB, m_ParamSb[0]);
	DDX_Control(pDX, IDC_HGT_PARAM2SB, m_ParamSb[1]);
	DDX_Control(pDX, IDC_HGT_PARAM3SB, m_ParamSb[2]);
	DDX_Control(pDX, IDC_HGT_PARAM4SB, m_ParamSb[3]);
	DDX_Control(pDX, IDC_HGT_PARAM5SB, m_ParamSb[4]);
	DDX_Text(pDX, IDC_HGT_SCALEEB, m_iScale);
	DDX_Control(pDX, IDC_HGT_SCALESB, m_ScaleSb);
	DDX_Check(pDX, IDC_HGT_GENERATECK, m_bGenerate);
	DDX_Control(pDX, IDC_HGT_PREVIEW, m_Preview);
}


BEGIN_MESSAGE_MAP(CHeightmapDlg, CDialog)
	ON_BN_CLICKED(IDC_HGT_GENERATECK, &CHeightmapDlg::OnBnClickedHgtGenerateck)
	ON_BN_CLICKED(IDC_HGT_FILEBTN, &CHeightmapDlg::OnBnClickedHgtFilebtn)
	ON_BN_CLICKED(IDC_HGT_GENERATEBTN, &CHeightmapDlg::OnBnClickedHgtGeneratebtn)
	ON_BN_CLICKED(IDC_HGT_SMOOTHBTN, &CHeightmapDlg::OnBnClickedHgtSmoothbtn)
	ON_BN_CLICKED(IDC_HGT_SCALEBTN, &CHeightmapDlg::OnBnClickedHgtScalebtn)
	ON_BN_CLICKED(IDC_HGT_PREV, &CHeightmapDlg::OnBnClickedHgtPrev)
	ON_BN_CLICKED(IDC_HGT_NEXT, &CHeightmapDlg::OnBnClickedHgtNext)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_SHOWWINDOW()
	ON_CBN_SELCHANGE(IDC_HGT_GENTYPECB, &CHeightmapDlg::OnCbnSelchangeHgtGentypecb)
END_MESSAGE_MAP()


// Gestionnaires de messages de CHeightmapDlg

BOOL CHeightmapDlg::OnInitDialog()
{
	if(!CDialog::OnInitDialog())
		return FALSE;

	m_ScaleSb.SetRange(1,255);
	m_ScaleSb.SetPos(10);

	Reinit();

	return TRUE;
}

void CHeightmapDlg::Reinit()
{
	m_bGenerate=FALSE;
	m_iGenType=0;
	m_ScaleSb.SetPos(10);
	UpdateData(FALSE);

	GetDlgItem(IDC_HGT_FILEBTN)->SetWindowText("<Heightmap>");

	if(m_pCurrentGenerator) delete m_pCurrentGenerator;
	m_pCurrentGenerator=new PerlinGenerator;

	ShowHParams(m_pCurrentGenerator);
	EnableHeightmapParameters(FALSE);
}

void CHeightmapDlg::OnOK()
{
}

void CHeightmapDlg::OnCancel()
{
}

void CHeightmapDlg::OnBnClickedHgtGenerateck()
{
	UpdateData(TRUE);
	EnableHeightmapParameters(m_bGenerate);
}

void CHeightmapDlg::OnBnClickedHgtFilebtn()
{
	g_FileSystem.ChangeWorkingDirectory("Maps\\");
	CString strPath(g_FileSystem.GetWorkingDirectory());
	strPath+="\\*.bmp; *.hgt";

	CFileDialog dialog(TRUE,"*.bmp",strPath,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"Bitmap (*.bmp)|*.bmp|DeathChase Heightmap (*.hgt)|*.hgt||");
	if(dialog.DoModal()==IDCANCEL)
	{
		g_FileSystem.ResetWorkingDirectory();
		return;
	}

	const int iCurrentSize=g_Map->GetHeightmap()->GetSize();

	if(!g_Map->GetHeightmap()->Load(dialog.GetFileName(),""))
	{
		g_Map->GetHeightmap()->CreateFlat(iCurrentSize);
		MessageBox("Erreur lors du chargement de la heightmap: le format n'est peut être pas bon (8 ou 24 bits pour les .bmp)","Erreur",MB_OK | MB_ICONERROR);
		GetDlgItem(IDC_HGT_FILEBTN)->SetWindowText("<Heightmap>");
	}
	else
		GetDlgItem(IDC_HGT_FILEBTN)->SetWindowText(dialog.GetFileTitle());

	g_FileSystem.ResetWorkingDirectory();

	if(g_Map->GetHeightmap()->GetSize()!=iCurrentSize)
	{
		const int iNewSize=g_Map->GetHeightmap()->GetSize();
		g_TextureMgr->ReleaseTexture(m_hTexture);
		m_hTexture=g_TextureMgr->CreateEmptyTexture(iNewSize,iNewSize,D3DFMT_A8R8G8B8,"HeightmapPreview");
	}

	if(g_Map->GetHeightmap()->GetScale()!=0.0f)
	{
		m_iScale=(int)g_Map->GetHeightmap()->GetScale();
		UpdateData(FALSE);
	}

	g_WindowMgr.GetDoc()->UpdateAllViews(0);
	FillTexture(true);
}

void CHeightmapDlg::OnBnClickedHgtGeneratebtn()
{
	UpdateData(TRUE);

	int i;
	for(i=0;i<m_iHParamCount;i++)
		*m_pHGeneratorValues[i]=(float)m_ParamSb[i].GetPos();

	if(m_pCurrentGenerator)
	{
		g_Map->GetHeightmap()->Unscale();
		m_pCurrentGenerator->GenerateHeightMap(g_Map->GetHeightmap()->GetSize(),g_Map->GetHeightmap()->GetMap());

		GetDlgItem(IDC_HGT_FILEBTN)->SetWindowText("<Heightmap>");
		g_WindowMgr.GetDoc()->UpdateAllViews(0);
		FillTexture(true);
	}
}

void CHeightmapDlg::OnBnClickedHgtSmoothbtn()
{
	g_Map->GetHeightmap()->Smooth();
	FillTexture(true);
	g_WindowMgr.GetDoc()->UpdateAllViews(0);
}

void CHeightmapDlg::OnBnClickedHgtScalebtn()
{
	UpdateData(TRUE);
	g_Map->GetHeightmap()->Scale((float)m_iScale);

	g_WindowMgr.GetDoc()->UpdateAllViews(0);
}

void CHeightmapDlg::OnBnClickedHgtPrev()
{
	g_WindowMgr.HideDialog(WT_HEIGHTMAP);
	g_WindowMgr.ShowDialog(WT_NEWMAP,false);
}

void CHeightmapDlg::OnBnClickedHgtNext()
{
	g_WindowMgr.HideDialog(WT_HEIGHTMAP);
	g_WindowMgr.ShowDialog(WT_TEXTURE,false);
}

void CHeightmapDlg::OnPaint()
{
	CPaintDC dc(this);

	if(!m_bMinimized)
		g_Renderer->DrawImageInView(m_hTexture,m_Preview.m_hWnd);
}

void CHeightmapDlg::OnSize(UINT nType, int cx, int cy)
{
	m_bMinimized=cx < 50 || cy < 50;
	CDialog::OnSize(nType, cx, cy);
}

void CHeightmapDlg::OnShowWindow(BOOL bShow, UINT nStatus)
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

		const int iSize=g_Map->GetHeightmap()->GetSize();
		m_hTexture=g_TextureMgr->CreateEmptyTexture(iSize,iSize,D3DFMT_A8R8G8B8,"HeightmapPreview");
		FillTexture(false);
	}
	else
	{
		g_TextureMgr->ReleaseTexture(m_hTexture);
		g_Renderer->RemoveView(m_Preview.m_hWnd);
	}
}

void CHeightmapDlg::OnCbnSelchangeHgtGentypecb()
{
	UpdateData(TRUE);

	IHeightMapGenerator *pGen=0;
	switch(m_iGenType)
	{
		case 0: pGen=new PerlinGenerator; break;
		case 1: pGen=new MidPointGenerator; break;
		case 2: pGen=new FaultFormationGenerator; break;
		default: return;
	}

	if(m_pCurrentGenerator) delete m_pCurrentGenerator;
	m_pCurrentGenerator=pGen;

	if(pGen)
		ShowHParams(pGen);
}

void CHeightmapDlg::ShowHParams(IHeightMapGenerator *pGenerator)
{
	if(!pGenerator)
		return;

	m_iHParamCount=pGenerator->GetParametersCount();
	uint32 i;
	char szName[32];
	float fMin,fMax,fStep;
	int iDecs;
	for(i=0;i<m_iHParamCount;i++)
	{
		pGenerator->GetParameter(i,szName,&m_pHGeneratorValues[i],fMin,fMax,fStep,iDecs);

		m_ParamSb[i].SetRange((int)fMin,(int)fMax);
		m_ParamSb[i].SetPos((int)*m_pHGeneratorValues[i]);
		m_ParamSt[i].SetWindowText(szName);

		m_ParamSb[i].ShowWindow(SW_SHOW);
		m_ParamSt[i].ShowWindow(SW_SHOW);
		m_ParamEb[i].ShowWindow(SW_SHOW);
	}

	for(;i<5;i++)
	{
		m_ParamSt[i].ShowWindow(SW_HIDE);
		m_ParamSb[i].ShowWindow(SW_HIDE);
		m_ParamEb[i].ShowWindow(SW_HIDE);
	}

	UpdateData(FALSE);
}

void CHeightmapDlg::EnableHeightmapParameters(BOOL bEnable)
{
	for(int i=0;i<m_iHParamCount;i++)
	{
		m_ParamSt[i].EnableWindow(bEnable);
		m_ParamEb[i].EnableWindow(bEnable);
		m_ParamSb[i].EnableWindow(bEnable);
	}
	
	GetDlgItem(IDC_HGT_GENTYPECB)->EnableWindow(bEnable);
	GetDlgItem(IDC_HGT_GENERATEBTN)->EnableWindow(bEnable);
	GetDlgItem(IDC_HGT_FILEBTN)->EnableWindow(!bEnable);
}

void CHeightmapDlg::FillTexture(bool bReleasePatchTex)
{
	const int iSize=g_Map->GetHeightmap()->GetSize();
	const float fScale=g_Map->GetHeightmap()->GetScale();
	const float *pMap=g_Map->GetHeightmap()->GetMap();

	Core::Color32 *pTemp=new Core::Color32[iSize*iSize];
	Core::Color32 *pDest=pTemp;

	if(fScale==0.0f)
	{
		for(int y=0;y<iSize;y++)
		{
			for(int x=0;x<iSize;x++)
			{
				pDest->a=255;
				pDest->r=pDest->g=pDest->b=(byte)*pMap;

				pDest++;
				pMap++;
			}

			pMap++; // la véritable taille de la heightmap est size+1 (pour la tesselation)
		}
	}
	else
	{
		const float fFact=255.0f / fScale;
		for(int y=0;y<iSize;y++)
		{
			for(int x=0;x<iSize;x++)
			{
				pDest->a=255;
				pDest->r=pDest->g=pDest->b=(byte)((*pMap) * fFact);

				pDest++;
				pMap++;
			}

			pMap++;
		}
	}

	g_TextureMgr->FillTexture(m_hTexture,(void *)pTemp,iSize,iSize,4);
	g_Map->GetPatchGrid()->Clear(bReleasePatchTex);
	g_Map->GetPatchGrid()->Create(g_Map->GetHeightmap(),16);
	g_Map->GetPatchGrid()->Generate();
	g_Map->GetPatchGrid()->Recalculate(0,0,2048);
	g_Renderer->DrawImageInView(m_hTexture,m_Preview.m_hWnd);
}
