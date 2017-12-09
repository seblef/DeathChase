// NewMap.cpp : fichier d'implémentation
//

#include "stdafx.h"
#include "DeathEditor.h"
#include "DeathEditorDoc.h"
#include "NewMap.h"
#include "WindowMgr.h"

#include "../DeathEngine/Export/IMap.h"


// Boîte de dialogue NewMap

IMPLEMENT_DYNAMIC(NewMap, CDialog)

NewMap::NewMap(CWnd* pParent /*=NULL*/)
	: CDialog(NewMap::IDD, pParent)
	, m_strName(_T(""))
	, m_iSize(0)
{

}

NewMap::~NewMap()
{
}

void NewMap::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_NEWMAPNAME, m_strName);
	DDX_CBIndex(pDX, IDC_NEWMAPSIZE, m_iSize);
}


BEGIN_MESSAGE_MAP(NewMap, CDialog)
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_NEWMAP_CANCEL, &NewMap::OnBnClickedNewmapCancel)
	ON_BN_CLICKED(IDC_NEWMAP_NEXT, &NewMap::OnBnClickedNewmapNext)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


// Gestionnaires de messages de NewMap

int NewMap::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

BOOL NewMap::OnInitDialog()
{
	if(!CDialog::OnInitDialog()) return FALSE;

	Reinit();

	return TRUE;
}

void NewMap::OnOK()
{
}

void NewMap::OnCancel()
{
	OnBnClickedNewmapCancel();
}

void NewMap::Reinit()
{
	m_iSize=0;
	m_strName="Nouvelle map";
	UpdateData(FALSE);
}

void NewMap::OnBnClickedNewmapCancel()
{
	((CDeathEditorDoc *)g_WindowMgr.GetDoc())->EnableMenus(true);
	((CDeathEditorDoc *)g_WindowMgr.GetDoc())->SetCurrentEditor(-1);
	g_WindowMgr.HideDialog(WT_NEWMAP);
	g_WindowMgr.ReinitAllDialogs();
}

void NewMap::OnBnClickedNewmapNext()
{
	UpdateData(TRUE);
	int iSize;

	switch(m_iSize)
	{
		case 0: iSize=256; break;
		case 1: iSize=512; break;
		case 2: iSize=1024; break;
		case 3: iSize=2048; break;
		default: iSize=256; break;
	}

	g_Map->CreateEmpty(m_strName,iSize);
	((CDeathEditorDoc *)g_WindowMgr.GetDoc())->SetDocumentName(m_strName);
	g_WindowMgr.GetDoc()->UpdateAllViews(0,UPDATE_NEWMAP);

	g_WindowMgr.HideDialog(WT_NEWMAP);
	g_WindowMgr.ShowDialog(WT_HEIGHTMAP,false);
}

void NewMap::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	if(bShow)
		((CDeathEditorDoc *)g_WindowMgr.GetDoc())->EnableMenus(false);
}
