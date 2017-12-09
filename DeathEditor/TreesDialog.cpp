// TreesDialog.cpp : fichier d'implémentation
//

#include "stdafx.h"
#include "DeathEditor.h"
#include "TreesDialog.h"

#include "EnvironmentDlg.h"
#include "DeathEditorDoc.h"
#include "FileSelectDlg.h"

#include "FileSystem.h"
#include "FileList.h"
#include "Interfaces.h"
#include "WindowMgr.h"
#include "../DeathEngine/Export/IRenderer.h"
#include "../DeathEngine/Export/IMap.h"
#include "../DeathEngine/Export/IMeshMgr.h"
#include "../DeathEngine/Export/IMesh.h"
#include "../DeathEngine/Export/ITreeGenerator.h"
#include "../DeathEngine/Game/TreeDefs.h"
#include "../DeathEngine/Dirs.h"
#include "../DeathEngine/Export/ICamera.h"
#include "../DeathEngine/Export/IFactory.h"
#include "../DeathEngine/Export/IHeightmap.h"




// Boîte de dialogue CTreesDialog

IMPLEMENT_DYNAMIC(CTreesDialog, CDialog)

CTreesDialog::CTreesDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CTreesDialog::IDD, pParent)
	, m_iMin(0)
	, m_iMax(0)
	, m_iDensity(100),
	m_bInited(false)
	, m_iScale(100)
	, m_bSelected(FALSE)
{

}

CTreesDialog::~CTreesDialog()
{
}

void CTreesDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREELIST, m_List);
	DDX_Control(pDX, IDC_TREEPREVIEW, m_Preview);
	DDX_Text(pDX, IDC_TREEMINEB, m_iMin);
	DDV_MinMaxInt(pDX, m_iMin, 0, 255);
	DDX_Text(pDX, IDC_TREEMAXEB, m_iMax);
	DDV_MinMaxInt(pDX, m_iMax, 0, 255);
	DDX_Control(pDX, IDC_TREEMINSB, m_MinSb);
	DDX_Control(pDX, IDC_TREEMAXSB, m_MaxSb);
	DDX_Text(pDX, IDC_TREEDENSITYEB, m_iDensity);
	DDX_Control(pDX, IDC_TREEDENSITYSB, m_DensitySb);
	DDX_Text(pDX, IDC_TREESCALEEB, m_iScale);
	DDX_Control(pDX, IDC_TREESCALESB, m_ScaleSb);
	DDX_Check(pDX, IDC_TREESELECTCK, m_bSelected);
}


BEGIN_MESSAGE_MAP(CTreesDialog, CDialog)
	ON_BN_CLICKED(IDC_TREEIMPORT, &CTreesDialog::OnBnClickedTreeimport)
	ON_BN_CLICKED(IDC_TREEAPPLY, &CTreesDialog::OnBnClickedTreeapply)
	ON_LBN_SELCHANGE(IDC_TREELIST, &CTreesDialog::OnLbnSelchangeTreelist)
	ON_EN_CHANGE(IDC_TREEMINEB, &CTreesDialog::OnEnChangeTreemineb)
	ON_EN_CHANGE(IDC_TREEMAXEB, &CTreesDialog::OnEnChangeTreemaxeb)
	ON_EN_CHANGE(IDC_TREEDENSITYEB, &CTreesDialog::OnEnChangeTreedensityeb)
	ON_WM_SHOWWINDOW()
	ON_WM_DESTROY()
	ON_LBN_DBLCLK(IDC_TREELIST, &CTreesDialog::OnLbnDblclkTreelist)
	ON_BN_CLICKED(IDC_TREESELECTCK, &CTreesDialog::OnBnClickedTreeselectck)
	ON_EN_CHANGE(IDC_TREESCALEEB, &CTreesDialog::OnEnChangeTreescaleeb)
	ON_BN_CLICKED(IDC_TREES_PREV, &CTreesDialog::OnBnClickedTreesPrev)
	ON_BN_CLICKED(IDC_TREES_NEXT, &CTreesDialog::OnBnClickedTreesNext)
END_MESSAGE_MAP()

BOOL CTreesDialog::OnInitDialog()
{
	if(!CDialog::OnInitDialog())
		return FALSE;

	m_MinSb.SetRange(0,255);
	m_MaxSb.SetRange(0,255);
	m_ScaleSb.SetRange(1,10000);
	m_DensitySb.SetRange(0,255);

	m_bInited=true;

	CRect rect;
	m_Preview.GetClientRect(&rect);

	m_Preview.GetCamera()->Initialize(CAM_PERSPECTIVE,0.5f,5000.0f,(float)rect.Width()/2,(float)rect.Height()/2,90,1.0f);

	for(int i=0;i<TREE_COUNT;i++)
	{
		TreeItem item;
		Core::HMesh hBurned;
		if(!g_Map->GetTreeGenerator()->GetTreeMesh((TreeType)i,item.hMesh,hBurned))
			g_Output2->AddMsg("x Erreur à la création de l'arbre %s.\n",szTreeNames[i]);
		item.iMin=0;
		item.iMax=255;
		item.fScale=1.0f;
		item.m_bSelected=false;
		strcpy(item.szName,szTreeNames[i]);

		m_Trees.push_back(item);
		m_List.AddString(szTreeNames[i]);
	}

	g_FileSystem.ChangeWorkingDirectory(DIR_TREES);

	FileList flist;
	FileList sub;
	std::vector<std::string> filters;
	filters.push_back(std::string(".dgeo"));
	flist.Reset(&filters);
	std::string strName;

	for(int i=0;i<flist.GetFileCount();i++)
	{
		if(flist.IsDirectory(i))
		{
			if(strcmp(flist.GetFileName(i),".")==0 || strcmp(flist.GetFileName(i),"..")==0)
				continue;

			g_FileSystem.ChangeWorkingDirectory(flist.GetFileName(i));
			sub.Reset(&filters,true);
			g_FileSystem.ChangeWorkingDirectory("..");

			for(int j=0;j<sub.GetFileCount();j++)
			{
				strName=sub.GetFileName(j);
				strName=strName.substr(0,strName.length()-5); // 5=".dgeo"

				TreeItem item;
				item.fScale=1.0f;
				item.iMin=0;
				item.iMax=255;
				item.m_bSelected=false;
				item.hMesh=g_MeshMgr->LoadMesh(strName.c_str(),"");
				if(!item.hMesh.IsNull())
				{
					strcpy(item.szName,strName.c_str());
					m_Trees.push_back(item);
					m_List.AddString(strName.c_str());
				}

				g_FileSystem.ChangeWorkingDirectory(DIR_TREES);
			}
		}
		else
		{
			strName=flist.GetFileName(i);
			strName=strName.substr(0,strName.length()-5);

			TreeItem item;
			item.fScale=1.0f;
			item.iMin=0;
			item.iMax=255;
			item.m_bSelected=false;
			item.hMesh=g_MeshMgr->LoadMesh(strName.c_str(),"");
			if(!item.hMesh.IsNull())
			{
				strcpy(item.szName,strName.c_str());
				m_Trees.push_back(item);
				m_List.AddString(strName.c_str());
			}
		}
	}

	g_FileSystem.ResetWorkingDirectory();

	return TRUE;
}

void CTreesDialog::Reinit()
{
	m_bSelected=FALSE;
	UpdateData(FALSE);
	m_List.SetCurSel(-1);

	for(int i=0;i<m_Trees.size();i++)
	{
		m_Trees[i].iMin=0;
		m_Trees[i].iMax=255;
		m_Trees[i].fScale=1.0f;
		m_Trees[i].m_bSelected=false;
	}

	CRect rect;
	m_Preview.GetClientRect(&rect);
	m_Preview.GetCamera()->Initialize(CAM_PERSPECTIVE,0.5f,5000.0f,(float)rect.Width()/2,(float)rect.Height()/2,90,1.0f);
}

void CTreesDialog::OnOK()
{
}

void CTreesDialog::OnCancel()
{
	g_WindowMgr.HideDialog(WT_TREES);
}


// Gestionnaires de messages de CTreesDialog

void CTreesDialog::OnBnClickedTreeimport()
{
	CFileSelectDlg dlg;
	dlg.SetFileType(FILE_MESH);
	if(dlg.DoModal()==IDCANCEL)
		return;

	Core::HMesh hMesh=g_MeshMgr->LoadMesh(dlg.m_strFileName,dlg.m_strPathName);
	if(hMesh.IsNull())
		return;

	IMesh *pMesh=g_MeshMgr->GetMesh(hMesh);

	TreeItem item;
	item.fScale=1.0f;
	item.iMin=0;
	item.iMax=255;
	item.m_bSelected=true;
	item.hMesh=hMesh;
	strcpy(item.szName,pMesh->GetName());

	m_Trees.push_back(item);
	m_List.AddString(pMesh->GetName());
	m_List.SetCurSel(m_List.GetCount()-1);
	OnLbnSelchangeTreelist();
}

void CTreesDialog::OnBnClickedTreeapply()
{
	UpdateData(TRUE);

	if(m_Trees.size()==0) return;

	m_Placement.Clear();
	std::vector<TreeItem>::iterator i;
	for(i=m_Trees.begin();i!=m_Trees.end();i++)
	{
		if(i->m_bSelected)
			m_Placement.AddTreeTemplate(i->szName,i->iMin,i->iMax,i->fScale);
	}

	const int iCount=m_iDensity * g_Map->GetHeightmap()->GetSize() / 255;
	m_Placement.Generate(iCount);

	g_WindowMgr.GetDoc()->UpdateAllViews(0);
}


void CTreesDialog::OnLbnSelchangeTreelist()
{
	const int iSelected=m_List.GetCurSel();
	if(iSelected!=-1)
	{
		m_iMin=m_Trees[iSelected].iMin;
		m_iMax=m_Trees[iSelected].iMax;
		m_iScale=(int)(m_Trees[iSelected].fScale * 100.0f);
		m_bSelected=m_Trees[iSelected].m_bSelected ? TRUE : FALSE;
		m_Preview.SetMesh(m_Trees[iSelected].hMesh);
		UpdateData(FALSE);
	}
	else
		m_Preview.SetMesh(Core::HMesh());

	m_Preview.Draw();
}

void CTreesDialog::OnEnChangeTreemineb()
{
	if(m_bInited)
	{
		const int iSelected=m_List.GetCurSel();
		UpdateData(TRUE);

		if(iSelected!=-1) m_Trees[iSelected].iMin=m_iMin;
	}
}

void CTreesDialog::OnEnChangeTreemaxeb()
{
	if(m_bInited)
	{
		const int iSelected=m_List.GetCurSel();
		UpdateData(TRUE);

		if(iSelected!=-1) m_Trees[iSelected].iMax=m_iMax;
	}
}

void CTreesDialog::OnEnChangeTreedensityeb()
{
	if(m_bInited) UpdateData(TRUE);
}

void CTreesDialog::OnShowWindow(BOOL bShow, UINT nStatus)
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

void CTreesDialog::OnDestroy()
{
	CDialog::OnDestroy();
}

void CTreesDialog::OnLbnDblclkTreelist()
{
	const int iSelected=m_List.GetCurSel();
	if(iSelected==-1) return;

	if(m_Trees[iSelected].m_bSelected)
	{
		m_List.DeleteString(iSelected);
		m_List.InsertString(iSelected,m_Trees[iSelected].szName);
		m_Trees[iSelected].m_bSelected=false;
	}
	else
	{
		CString strItem("x ");
		strItem+=m_Trees[iSelected].szName;
		m_List.DeleteString(iSelected);
		m_List.InsertString(iSelected,strItem);
		m_Trees[iSelected].m_bSelected=true;
	}

	m_bSelected=m_Trees[iSelected].m_bSelected ? TRUE : FALSE;
	UpdateData(FALSE);
}

void CTreesDialog::OnBnClickedTreeselectck()
{
	UpdateData(TRUE);

	const int iSelected=m_List.GetCurSel();
	if(iSelected!=-1)
	{
		if(!m_bSelected)
		{
			m_List.DeleteString(iSelected);
			m_List.InsertString(iSelected,m_Trees[iSelected].szName);
			m_Trees[iSelected].m_bSelected=false;
		}
		else
		{
			CString strItem("x ");
			strItem+=m_Trees[iSelected].szName;
			m_List.DeleteString(iSelected);
			m_List.InsertString(iSelected,strItem);
			m_Trees[iSelected].m_bSelected=true;
		}
	}
}

void CTreesDialog::OnEnChangeTreescaleeb()
{
	if(m_bInited)
	{
		const int iSelected=m_List.GetCurSel();
		if(iSelected!=-1)
		{
			UpdateData(TRUE);
			m_Trees[iSelected].fScale=(float)m_iScale * 0.01f;
		}
	}
}

void CTreesDialog::OnBnClickedTreesPrev()
{
	g_WindowMgr.HideDialog(WT_TREES);
	if(((CEnvironmentDlg *)g_WindowMgr.GetDialog(WT_ENVIRONMENT))->m_bGenerateWater)
		g_WindowMgr.ShowDialog(WT_WATER,false);
	else
		g_WindowMgr.ShowDialog(WT_ENVIRONMENT,false);
}

void CTreesDialog::OnBnClickedTreesNext()
{
	g_WindowMgr.HideDialog(WT_TREES);
	g_WindowMgr.ReinitAllDialogs();
	g_WindowMgr.GetDoc()->UpdateAllViews(0,UPDATE_MAPCREATED);
	((CDeathEditorDoc *)g_WindowMgr.GetDoc())->EnableMenus(true);
}
