// TextureDlg.cpp : fichier d'implémentation
//

#include "stdafx.h"
#include "DeathEditor.h"
#include "TextureDlg.h"

#include "FileSystem.h"
#include "WindowMgr.h"
#include "Interfaces.h"
#include "Data/TexGenerator.h"
#include "../DeathEngine/Export/IRenderer.h"
#include "../DeathEngine/Export/ITextureMgr.h"
#include "../DeathEngine/Export/IMap.h"
#include "../DeathEngine/Export/IPatchGrid.h"
#include "../DeathEngine/Export/IHeightmap.h"

// Boîte de dialogue CTextureDlg

IMPLEMENT_DYNAMIC(CTextureDlg, CDialog)

CTextureDlg::CTextureDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTextureDlg::IDD, pParent)
	, m_bGenerate(FALSE)
	, m_iSize(0)
	, m_iMin(0)
	, m_iMax(0),
	m_bInited(false),
	m_bMinimized(false)
{

}

CTextureDlg::~CTextureDlg()
{
	g_TextureMgr->ReleaseTexture(m_hTexture);
}

void CTextureDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_TEX_GENERATECK, m_bGenerate);
	DDX_CBIndex(pDX, IDC_TEX_SIZECB, m_iSize);
	DDX_Control(pDX, IDC_TEX_TEXLIST, m_TexList);
	DDX_Text(pDX, IDC_TEX_MINEB, m_iMin);
	DDX_Text(pDX, IDC_TEX_MAXEB, m_iMax);
	DDX_Control(pDX, IDC_TEX_PREVIEW, m_Preview);
	DDX_Control(pDX, IDC_TEX_MINSB, m_MinSb);
	DDX_Control(pDX, IDC_TEX_MAXSB, m_MaxSb);
}


BEGIN_MESSAGE_MAP(CTextureDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_SHOWWINDOW()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_TEX_GENERATECK, &CTextureDlg::OnBnClickedTexGenerateck)
	ON_BN_CLICKED(IDC_TEX_FILEBTN, &CTextureDlg::OnBnClickedTexFilebtn)
	ON_CBN_SELCHANGE(IDC_TEX_SIZECB, &CTextureDlg::OnCbnSelchangeTexSizecb)
	ON_LBN_SELCHANGE(IDC_TEX_TEXLIST, &CTextureDlg::OnLbnSelchangeTexTexlist)
	ON_BN_CLICKED(IDC_TEX_ADDBTN, &CTextureDlg::OnBnClickedTexAddbtn)
	ON_BN_CLICKED(IDC_TEX_REMBTN, &CTextureDlg::OnBnClickedTexRembtn)
	ON_EN_CHANGE(IDC_TEX_MINEB, &CTextureDlg::OnEnChangeTexMineb)
	ON_EN_CHANGE(IDC_TEX_MAXEB, &CTextureDlg::OnEnChangeTexMaxeb)
	ON_BN_CLICKED(IDC_TEX_TEXBTN, &CTextureDlg::OnBnClickedTexTexbtn)
	ON_BN_CLICKED(IDC_TEX_GENERATEBTN, &CTextureDlg::OnBnClickedTexGeneratebtn)
	ON_BN_CLICKED(IDC_TEX_DETAILBTN, &CTextureDlg::OnBnClickedTexDetailbtn)
	ON_BN_CLICKED(IDC_TEX_PREVBTN, &CTextureDlg::OnBnClickedTexPrevbtn)
	ON_BN_CLICKED(IDC_TEX_NEXTBTN, &CTextureDlg::OnBnClickedTexNextbtn)
	ON_STN_CLICKED(IDC_TEX_PREVIEW, &CTextureDlg::OnStnClickedTexPreview)
END_MESSAGE_MAP()


// Gestionnaires de messages de CTextureDlg

BOOL CTextureDlg::OnInitDialog()
{
	if(!CDialog::OnInitDialog())
		return FALSE;

	m_MinSb.SetRange(0,255);
	m_MaxSb.SetRange(0,255);

	Reinit();
	m_bInited=true;

	return TRUE;
}

void CTextureDlg::Reinit()
{
	GetDlgItem(IDC_TEX_FILEBTN)->SetWindowText("<Texture>");
	GetDlgItem(IDC_TEX_DETAILBTN)->SetWindowText("<Detail>");
	GetDlgItem(IDC_TEX_TEXBTN)->SetWindowText("<Texture>");

	m_TexList.ResetContent();
	m_iMin=0;
	m_iMax=255;
	UpdateData(FALSE);

	m_hTexture=Core::HTexture();
	for(int i=0;i<m_TexLevels.size();i++)
		g_TextureMgr->ReleaseTexture(m_TexLevels[i].hTexture);

	m_TexLevels.clear();

	if(m_bInited)
		EnableParameters(FALSE);
}

void CTextureDlg::OnOK()
{
}

void CTextureDlg::OnCancel()
{
}

void CTextureDlg::OnPaint()
{
	CPaintDC dc(this);
	
	if(!m_bMinimized)
		g_Renderer->DrawImageInView(m_hCurrentTexture,m_Preview.m_hWnd);
}

void CTextureDlg::OnShowWindow(BOOL bShow, UINT nStatus)
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
	}
	else
		g_Renderer->RemoveView(m_Preview.m_hWnd);
}

void CTextureDlg::OnSize(UINT nType, int cx, int cy)
{
	m_bMinimized=cx < 50 || cy < 50;
	CDialog::OnSize(nType, cx, cy);
}

void CTextureDlg::OnBnClickedTexGenerateck()
{
	UpdateData(TRUE);
	EnableParameters(m_bGenerate);
}

void CTextureDlg::OnBnClickedTexFilebtn()
{
	g_FileSystem.ChangeWorkingDirectory("Maps\\");
	CString strPath(g_FileSystem.GetWorkingDirectory());
	strPath+=IMAGE_EXTPATH;

	CFileDialog dialog(TRUE,"*.jpg",strPath,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,IMAGE_EXTENSIONS);
	if(dialog.DoModal()==IDCANCEL)
	{
		g_FileSystem.ResetWorkingDirectory();
		return;
	}

	Core::HTexture hTemp=g_TextureMgr->LoadTexture(dialog.GetFileName(),"");
	if(hTemp.IsNull())
	{
		MessageBox("Erreur lors du chargement de la texture.","Erreur",MB_OK | MB_ICONERROR);
		g_FileSystem.ResetWorkingDirectory();
		return;
	}

	g_TextureMgr->ReleaseTexture(m_hTexture);
	m_hTexture=hTemp;
	m_hCurrentTexture=hTemp;
	g_Map->GetPatchGrid()->SetTerrainTexture(m_hTexture);

	g_FileSystem.ResetWorkingDirectory();

	GetDlgItem(IDC_TEX_FILEBTN)->SetWindowText(dialog.GetFileName());
	g_Renderer->DrawImageInView(m_hCurrentTexture,m_Preview.m_hWnd);
	g_WindowMgr.GetDoc()->UpdateAllViews(0);
}

void CTextureDlg::OnCbnSelchangeTexSizecb()
{
	if(m_bInited) UpdateData(TRUE);
}

void CTextureDlg::OnLbnSelchangeTexTexlist()
{
	UpdateData(TRUE);

	const int iSelected=m_TexList.GetCurSel();
	if(iSelected==-1)
	{
		EnableTexParameters(FALSE);
		m_hCurrentTexture=m_hTexture;
		GetDlgItem(IDC_TEX_TEXBTN)->SetWindowText("<Texture>");
		return;
	}
	else
	{
		EnableTexParameters(TRUE);

		if(iSelected < m_TexLevels.size())
		{
			m_MinSb.SetPos(m_TexLevels[iSelected].iMin);
			m_MaxSb.SetPos(m_TexLevels[iSelected].iMax);
			m_hCurrentTexture=m_TexLevels[iSelected].hTexture;
			CString strTemp;
			m_TexList.GetText(iSelected,strTemp);
			GetDlgItem(IDC_TEX_TEXBTN)->SetWindowText(strTemp);
			UpdateData(FALSE);
		}
	}

	g_Renderer->DrawImageInView(m_hCurrentTexture,m_Preview.m_hWnd);
}

void CTextureDlg::OnBnClickedTexAddbtn()
{
	UpdateData(TRUE);

	g_FileSystem.ChangeWorkingDirectory("Textures\\");
	CString strPath(g_FileSystem.GetWorkingDirectory());
	strPath+="\\*.bmp";

	CFileDialog dialog(TRUE,"*.bmp",strPath,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"Bitmap (*.bmp)|*.bmp||");
	if(dialog.DoModal()==IDCANCEL)
	{
		g_FileSystem.ResetWorkingDirectory();
		return;
	}

	TexGenLevel lev;
	lev.hTexture=g_TextureMgr->LoadTexture(dialog.GetFileName(),"");

	if(lev.hTexture.IsNull())
	{
		MessageBox("Erreur lors du chargement de la texture.","Erreur",MB_OK | MB_ICONERROR);
		g_FileSystem.ResetWorkingDirectory();
		return;
	}

	lev.strPath=g_FileSystem.GetWorkingDirectory();
	lev.strPath+="\\";
	g_FileSystem.ResetWorkingDirectory();

	lev.iMin=0;
	lev.iMax=255;
	lev.strTexture=dialog.GetFileName();

	m_TexLevels.push_back(lev);
	GetDlgItem(IDC_TEX_TEXBTN)->SetWindowText(dialog.GetFileTitle());
	m_TexList.AddString(dialog.GetFileTitle());
	m_TexList.SetCurSel(m_TexList.GetCount()-1);
	m_hCurrentTexture=lev.hTexture;
	m_iMin=0;
	m_iMax=255;
	GetDlgItem(IDC_TEX_GENERATEBTN)->EnableWindow(TRUE);
	UpdateData(FALSE);
	EnableTexParameters(TRUE);
	g_Renderer->DrawImageInView(m_hCurrentTexture,m_Preview.m_hWnd);
}

void CTextureDlg::OnBnClickedTexRembtn()
{
	const int iSelected=m_TexList.GetCurSel();
	if(iSelected==-1 || iSelected >= m_TexLevels.size())
		return;

	g_TextureMgr->ReleaseTexture(m_TexLevels[iSelected].hTexture);
	m_TexLevels.erase(m_TexLevels.begin()+iSelected);
	m_TexList.DeleteString(iSelected);
	m_TexList.SetCurSel(-1);
	m_hCurrentTexture=m_hTexture;
	EnableTexParameters(FALSE);
	GetDlgItem(IDC_TEX_TEXBTN)->SetWindowText("<Texture>");

	if(m_TexList.GetCount()==0) GetDlgItem(IDC_TEX_GENERATEBTN)->EnableWindow(FALSE);

	g_Renderer->DrawImageInView(m_hCurrentTexture,m_Preview.m_hWnd);
}

void CTextureDlg::OnEnChangeTexMineb()
{
	if(!m_bInited) return;

	const int iSelected=m_TexList.GetCurSel();

	if(iSelected!=-1 && iSelected < m_TexLevels.size())
	{
		UpdateData(TRUE);
		m_TexLevels[iSelected].iMin=m_iMin;
	}
}

void CTextureDlg::OnEnChangeTexMaxeb()
{
	if(!m_bInited) return;

	const int iSelected=m_TexList.GetCurSel();

	if(iSelected!=-1 && iSelected < m_TexLevels.size())
	{
		UpdateData(TRUE);
		m_TexLevels[iSelected].iMax=m_iMax;
	}
}

void CTextureDlg::OnBnClickedTexTexbtn()
{
	const int iSelected=m_TexList.GetCurSel();
	if(iSelected==-1 || iSelected >= m_TexLevels.size()) return;

	g_FileSystem.ChangeWorkingDirectory("Textures\\");
	CString strPath(g_FileSystem.GetWorkingDirectory());
	strPath+="\\*.bmp";

	CFileDialog dialog(TRUE,"*.bmp",strPath,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"Bitmap (*.bmp)|*.bmp||");
	if(dialog.DoModal()==IDCANCEL)
	{
		g_FileSystem.ResetWorkingDirectory();
		return;
	}

	if(!m_TexLevels[iSelected].hTexture.IsNull())
		g_TextureMgr->ReleaseTexture(m_TexLevels[iSelected].hTexture);

	m_TexLevels[iSelected].hTexture=g_TextureMgr->LoadTexture(dialog.GetFileName(),"");
	m_TexLevels[iSelected].strPath=g_FileSystem.GetWorkingDirectory();
	m_TexLevels[iSelected].strPath+="\\";

	if(m_TexLevels[iSelected].hTexture.IsNull())
	{
		MessageBox("Erreur lors du chargement de la texture.","Erreur",MB_OK | MB_ICONERROR);
		return;
	}

	m_TexLevels[iSelected].strTexture=dialog.GetFileName();

	GetDlgItem(IDC_TEX_TEXBTN)->SetWindowText(dialog.GetFileTitle());
	m_TexList.DeleteString(iSelected);
	m_TexList.InsertString(iSelected,dialog.GetFileTitle());
	m_TexList.SetCurSel(iSelected);
	m_hCurrentTexture=m_TexLevels[iSelected].hTexture;
	g_Renderer->DrawImageInView(m_hCurrentTexture,m_Preview.m_hWnd);
}

void CTextureDlg::OnBnClickedTexGeneratebtn()
{
	UpdateData(TRUE);

	int iSize;
	switch(m_iSize)
	{
		case 0: iSize=256; break;
		case 1: iSize=512; break;
		case 2: iSize=1024; break;
		case 3: iSize=2048; break;
		default: iSize=512; break;
	}

	g_TextureMgr->ReleaseTexture(m_hTexture);
	m_hTexture=g_TextureMgr->CreateEmptyTexture(iSize,iSize,D3DFMT_X8R8G8B8,"TerrainBase");

	if(m_hTexture.IsNull())
	{
		MessageBox("Erreur lors de la création de la texture.","Erreur",MB_OK | MB_ICONERROR);
		return;
	}

	const float fScale=g_Map->GetHeightmap()->GetScale();

	if(fScale!=0.0f)
		g_Map->GetHeightmap()->Unscale();

	TextureGenerator gen(g_Map->GetHeightmap());
	if(!gen.Generate(m_hTexture,iSize,&m_TexLevels,1))
		MessageBox("Erreur lors de la création de la texture.","Erreur",MB_OK | MB_ICONERROR);

	if(fScale!=0.0f)
		g_Map->GetHeightmap()->Scale(fScale);

	g_Map->GetPatchGrid()->SetTerrainTexture(m_hTexture);

	m_hCurrentTexture=m_hTexture;
	g_Renderer->DrawImageInView(m_hTexture,m_Preview.m_hWnd);
	g_WindowMgr.GetDoc()->UpdateAllViews(0);
}

void CTextureDlg::OnBnClickedTexDetailbtn()
{
	g_FileSystem.ChangeWorkingDirectory("Maps\\");
	CString strPath(g_FileSystem.GetWorkingDirectory());
	strPath+="\\*.bmp; *.jpg; *.tga; *.png";

	CFileDialog dialog(TRUE,"*.jpg",strPath,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,IMAGE_EXTENSIONS);
	if(dialog.DoModal()==IDCANCEL)
	{
		g_FileSystem.ResetWorkingDirectory();
		return;
	}

	Core::HTexture hDetail=g_TextureMgr->LoadTexture(dialog.GetFileName(),"");
	g_FileSystem.ResetWorkingDirectory();

	if(hDetail.IsNull())
	{
		MessageBox("Erreur au chargement de la texture.","Erreur",MB_OK | MB_ICONERROR);
		return;
	}

	g_TextureMgr->ReleaseTexture(g_Map->GetPatchGrid()->GetDetailTexture());
	g_TextureMgr->GenerateTextureMipMaps(hDetail);
	GetDlgItem(IDC_TEX_DETAILBTN)->SetWindowText(dialog.GetFileName());
	g_Map->GetPatchGrid()->SetDetailTexture(hDetail);

	m_hCurrentTexture=hDetail;
	g_Renderer->DrawImageInView(m_hCurrentTexture,m_Preview.m_hWnd);
	g_WindowMgr.GetDoc()->UpdateAllViews(0);
}

void CTextureDlg::OnBnClickedTexPrevbtn()
{
	g_WindowMgr.HideDialog(WT_TEXTURE);
	g_WindowMgr.ShowDialog(WT_HEIGHTMAP,false);
}

void CTextureDlg::OnBnClickedTexNextbtn()
{
	g_WindowMgr.HideDialog(WT_TEXTURE);
	g_WindowMgr.ShowDialog(WT_ENVIRONMENT,false);
}

void CTextureDlg::EnableParameters(BOOL bEnable)
{
	const int iSelected=m_TexList.GetCurSel();
	const int iTexCount=m_TexList.GetCount();

	GetDlgItem(IDC_TEX_FILEBTN)->EnableWindow(!bEnable);
	GetDlgItem(IDC_TEX_SIZECB)->EnableWindow(bEnable);
	GetDlgItem(IDC_TEX_TEXLIST)->EnableWindow(bEnable);
	GetDlgItem(IDC_TEX_ADDBTN)->EnableWindow(bEnable);
	EnableTexParameters(bEnable && iSelected!=-1);
	GetDlgItem(IDC_TEX_GENERATEBTN)->EnableWindow(bEnable && iTexCount > 0);
}

void CTextureDlg::OnStnClickedTexPreview()
{
	m_hCurrentTexture=m_hTexture;
}

void CTextureDlg::EnableTexParameters(BOOL bEnable)
{
	GetDlgItem(IDC_TEX_REMBTN)->EnableWindow(bEnable);
	
	GetDlgItem(IDC_TEX_MINEB)->EnableWindow(bEnable);
	GetDlgItem(IDC_TEX_MINSB)->EnableWindow(bEnable);

	GetDlgItem(IDC_TEX_MAXEB)->EnableWindow(bEnable);
	GetDlgItem(IDC_TEX_MAXSB)->EnableWindow(bEnable);

	GetDlgItem(IDC_TEX_TEXBTN)->EnableWindow(bEnable);
}