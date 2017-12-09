// FileSelectDlg.cpp : fichier d'implémentation
//

#include "stdafx.h"
#include "DeathEditor.h"
#include "FileSelectDlg.h"

#include "MeshEditDlg.h"

#include "Interfaces.h"
#include "../DeathEngine/Export/IMeshMgr.h"
#include "../DeathEngine/Export/IMesh.h"

// Boîte de dialogue CFileSelectDlg

IMPLEMENT_DYNAMIC(CFileSelectDlg, CDialog)

CFileSelectDlg::CFileSelectDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFileSelectDlg::IDD, pParent),
	m_Type(FILE_TEXTURE),
	m_strFileName(""),
	m_strPathName(""),
	m_pRoot(0),
	m_pSelected(0)
{

}

CFileSelectDlg::~CFileSelectDlg()
{
}

void CFileSelectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FILE_TREE, m_Tree);
}


BEGIN_MESSAGE_MAP(CFileSelectDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CFileSelectDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CFileSelectDlg::OnBnClickedCancel)
	ON_NOTIFY(TVN_SELCHANGED, IDC_FILE_TREE, &CFileSelectDlg::OnTvnSelchangedFileTree)
	ON_BN_CLICKED(IDC_FILE_NEW, &CFileSelectDlg::OnBnClickedFileNew)
	ON_BN_CLICKED(IDC_FILE_IMPORT, &CFileSelectDlg::OnBnClickedFileImport)
	ON_BN_CLICKED(IDC_FILE_REFRESH, &CFileSelectDlg::OnBnClickedFileRefresh)
	ON_BN_CLICKED(IDC_FILE_EDITBTN, &CFileSelectDlg::OnBnClickedFileEditbtn)
END_MESSAGE_MAP()


// Gestionnaires de messages de CFileSelectDlg

BOOL CFileSelectDlg::OnInitDialog()
{
	if(!CDialog::OnInitDialog())
		return FALSE;

	BOOL bEnable;

	switch(m_Type)
	{
		case FILE_TEXTURE:
		case FILE_MAP:
		case FILE_HEIGHTMAP:
		case FILE_SOUND:
		case FILE_SHADER:
			bEnable=FALSE;
			break;

		default:
			bEnable=TRUE;
			break;
	}

	GetDlgItem(IDC_FILE_NEW)->EnableWindow(bEnable);
	GetDlgItem(IDOK)->EnableWindow(FALSE);

	RebuildTree();

	return TRUE;
}

void CFileSelectDlg::OnBnClickedOk()
{
	if(m_pSelected)
	{
		m_strFileName=m_pSelected->strFileName.c_str();
		m_strPathName=m_pSelected->strPathName.c_str();
	}
	OnOK();
}

void CFileSelectDlg::OnBnClickedCancel()
{
	OnCancel();
}

void CFileSelectDlg::OnTvnSelchangedFileTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

	HTREEITEM hItem=m_Tree.GetSelectedItem();
	std::map<HTREEITEM,FileEntry *>::iterator i=m_TreeMap.find(hItem);

	if(i==m_TreeMap.end())
		m_pSelected=0;
	else
		m_pSelected=(i->second->Children.size() > 0) ? 0 : i->second;

	GetDlgItem(IDOK)->EnableWindow(m_pSelected!=0);

	*pResult = 0;
}

void CFileSelectDlg::OnBnClickedFileNew()
{
	if(m_Type==FILE_MESH)
	{
		CMeshEditDlg dlg;
		if(dlg.DoModal()==IDCANCEL) return;

		CString strDir(g_FileSystem.GetWorkingDirectory());
		IMesh *pMesh=g_MeshMgr->GetMesh(dlg.m_hMesh);

		if(!pMesh) return;

		strDir+="\\Models\\";
		strDir+=pMesh->GetName();
		strDir+=".dgeo";

		CFileDialog filedlg(FALSE,"*.*",strDir,OFN_EXPLORER,0,this);
		if(filedlg.DoModal()==IDCANCEL)
		{
			g_MeshMgr->ReleaseMesh(dlg.m_hMesh);
			return;
		}

		pMesh->Save(pMesh->GetName(),"");
		g_MeshMgr->ReleaseMesh(dlg.m_hMesh);

		g_FileSystem.RebuildFileList(FILE_MESH);
		RebuildTree();
	}
}

void CFileSelectDlg::OnBnClickedFileImport()
{
	const std::vector<std::string> *pFilters=g_FileSystem.GetFilters(m_Type);

	g_FileSystem.ResetWorkingDirectory();

	CString strExt("");
	CString strExtPath(g_FileSystem.GetWorkingDirectory());
	strExtPath+="\\";

	std::vector<std::string>::const_iterator i;
	for(i=pFilters->begin();i!=pFilters->end();i++)
	{
		strExt+=(*i).c_str();
		strExt+=" files (*";
		strExt+=(*i).c_str();
		strExt+=")|*";
		strExt+=(*i).c_str();
		strExt+="|";
		
		if(i!=pFilters->begin())
			strExtPath+="; *";
		else
			strExtPath+="*";

		strExtPath+=(*i).c_str();
	}

	CFileDialog filedlg(TRUE,0,strExtPath,OFN_HIDEREADONLY | OFN_FILEMUSTEXIST,strExt,this);
	if(filedlg.DoModal()==IDCANCEL) return;

	g_FileSystem.ResetWorkingDirectory();
	strExtPath=g_FileSystem.GetWorkingDirectory();
	strExtPath+="\\";
	strExtPath+=filedlg.GetFileName();
	
	CFileDialog dirdlg(FALSE,"*.*",strExtPath,OFN_EXPLORER,0,this);
	if(dirdlg.DoModal()==IDCANCEL) return;

	CString strSrcFile(filedlg.GetPathName());
	CString strDestPath(g_FileSystem.GetWorkingDirectory());
	strDestPath+="\\";
	CString strDestFile(strDestPath);
	strDestFile+=filedlg.GetFileName();

	if(m_Type==FILE_MESH)
	{
		Core::HMesh hMesh=g_MeshMgr->LoadMesh(strSrcFile,"");
		IMesh *pMesh=g_MeshMgr->GetMesh(hMesh);
		if(pMesh) pMesh->Save(pMesh->GetName(),strDestPath);
	}
	else
	{
		g_Output2->AddMsg("Copie %s a %s\n",(const char*)strSrcFile,(const char*)strDestFile);
		CopyFile(strSrcFile,strDestFile,FALSE);
	}

	g_FileSystem.RebuildFileList(m_Type);
	RebuildTree();
}

void CFileSelectDlg::OnBnClickedFileRefresh()
{
	g_FileSystem.RebuildFileList(m_Type);
	RebuildTree();
}

void CFileSelectDlg::RebuildTree()
{
	m_TreeMap.clear();
	m_Tree.DeleteAllItems();

	m_pRoot=g_FileSystem.GetFileList(m_Type);
	if(m_pRoot)
		RecursRebuildTree(m_pRoot,0);
}

void CFileSelectDlg::RecursRebuildTree(FileEntry *pEntry, HTREEITEM hParent)
{
	if(!pEntry) return;

	HTREEITEM hItem=m_Tree.InsertItem(pEntry->strFileName.c_str(),hParent);
	m_TreeMap.insert(std::make_pair(hItem,pEntry));

	std::vector<FileEntry *>::iterator i;
	for(i=pEntry->Children.begin();i!=pEntry->Children.end();i++)
		RecursRebuildTree(*i,hItem);

	if(hParent==0)
		m_Tree.Expand(hItem,TVE_EXPAND);
}
void CFileSelectDlg::OnBnClickedFileEditbtn()
{
	// TODO : ajoutez ici le code de votre gestionnaire de notification de contrôle
}
