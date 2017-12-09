

// Memory.cpp : fichier d'implémentation
//

#include "stdafx.h"
#include "DeathEditor.h"
#include "Memory.h"

#include "Interfaces.h"
#include "../DeathEngine/ConvertChar.h"

#ifdef DEDEV

// Boîte de dialogue CMemory

IMPLEMENT_DYNAMIC(CMemory, CDialog)

CMemory::CMemory(CWnd* pParent /*=NULL*/)
	: CDialog(CMemory::IDD, pParent)
{

}

CMemory::~CMemory()
{
}

void CMemory::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MEMTAB, m_Tab);
	DDX_Control(pDX, IDC_ALLOCLIST, m_AllocList);
	DDX_Control(pDX, IDC_MEMLIST, m_MemList);
}


BEGIN_MESSAGE_MAP(CMemory, CDialog)
	ON_BN_CLICKED(IDC_MEMREFRESH, &CMemory::OnBnClickedMemrefresh)
	ON_NOTIFY(TCN_SELCHANGE, IDC_MEMTAB, &CMemory::OnTcnSelchangeMemtab)
	ON_LBN_SELCHANGE(IDC_ALLOCLIST, &CMemory::OnLbnSelchangeAlloclist)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


// Gestionnaires de messages de CMemory

void CMemory::OnBnClickedMemrefresh()
{
	int iCurSel=m_AllocList.GetCurSel();
	m_AllocList.ResetContent();

	IMemAllocMgr *pAMgr=g_Engine->GetMemAllocMgr();
	if(pAMgr)
	{
		for(int i=0;i<pAMgr->GetAllocatorCount();i++)
		{
			int used,free;
			char szClassName[64];
			pAMgr->GetAllocatorInfos(i,szClassName,used,free);
			m_AllocList.AddString(szClassName);
		}
	}

	if(iCurSel!=1)
	{
		m_AllocList.SetCurSel(iCurSel);
		OnLbnSelchangeAlloclist();
	}

	m_MemList.DeleteAllItems();
	IMemDebugger *pMMgr=g_Engine->GetMemDebugger();
	if(pMMgr)
	{
		const MemDebuggerVector& items=pMMgr->GetItems();
		MemDebuggerVector::const_iterator i;
		int count=0;
		int total_new=0,total_del=0;
		int total_mnew=0,total_mdel=0;
		int total_size=0,total_extrasize=0;
		int total_inst=0,total_extra=0;
		int total_total=0,total_sizes=0;

		for(i=items.begin();i!=items.end();i++,count++)
		{
			const int index=m_MemList.InsertItem(count,i->szName);
			m_MemList.SetItemText(index,1,FormatSize(i->iSize));
			m_MemList.SetItemText(index,2,IntToChar(i->iNewCount));
			m_MemList.SetItemText(index,3,IntToChar(i->iDelCount));
			m_MemList.SetItemText(index,4,IntToChar(i->iMNewCount));
			m_MemList.SetItemText(index,5,IntToChar(i->iMDelCount));
			m_MemList.SetItemText(index,6,IntToChar(i->iInstances));
			m_MemList.SetItemText(index,7,FormatSize(i->iInstances * i->iSize));
			m_MemList.SetItemText(index,8,IntToChar(i->iExtraCount));
			m_MemList.SetItemText(index,9,FormatSize(i->iExtraSize));
			m_MemList.SetItemText(index,10,FormatSize(i->iTotalSize));

			total_new+=i->iNewCount;
			total_del+=i->iDelCount;
			total_mnew+=i->iMNewCount;
			total_mdel+=i->iMDelCount;
			total_size+=i->iSize;
			total_extra+=i->iExtraCount;
			total_extrasize+=i->iExtraSize;
			total_inst+=i->iInstances;
			total_total+=i->iTotalSize;
			total_sizes+=i->iInstances * i->iSize;
		}

		const int index=m_MemList.InsertItem(count,"Total");
		m_MemList.SetItemText(index,1,FormatSize(total_size));
		m_MemList.SetItemText(index,2,IntToChar(total_new));
		m_MemList.SetItemText(index,3,IntToChar(total_del));
		m_MemList.SetItemText(index,4,IntToChar(total_mnew));
		m_MemList.SetItemText(index,5,IntToChar(total_mdel));
		m_MemList.SetItemText(index,6,IntToChar(total_inst));
		m_MemList.SetItemText(index,7,FormatSize(total_sizes));
		m_MemList.SetItemText(index,8,IntToChar(total_extra));
		m_MemList.SetItemText(index,9,FormatSize(total_extrasize));
		m_MemList.SetItemText(index,10,FormatSize(total_total));
	}
}

const char *CMemory::FormatSize(const int iSize)
{
	static char szTemp[32];

	if(iSize < 4096)
		sprintf(szTemp,"%i o",iSize);
	else if(iSize < (4096*1024))
		sprintf(szTemp,"%i ko",iSize / 1024);
	else
		sprintf(szTemp,"%i mo",iSize / (1024 * 1024));

	return szTemp;
}

void CMemory::OnTcnSelchangeMemtab(NMHDR *pNMHDR, LRESULT *pResult)
{
	int iCmdA,iCmdM;

	if(m_Tab.GetCurSel()==1)
	{
		iCmdA=SW_HIDE;
		iCmdM=SW_SHOW;
	}
	else
	{
		iCmdA=SW_SHOW;
		iCmdM=SW_HIDE;
	}

	m_AllocList.ShowWindow(iCmdA);
	GetDlgItem(IDC_ALLOCUSED)->ShowWindow(iCmdA);
	GetDlgItem(IDC_ALLOCFREE)->ShowWindow(iCmdA);
	GetDlgItem(IDC_ALLOCTOTAL)->ShowWindow(iCmdA);

	m_MemList.ShowWindow(iCmdM);

	*pResult = 0;
}

void CMemory::OnLbnSelchangeAlloclist()
{
	const int iSelected=m_AllocList.GetCurSel();
	if(iSelected==-1) return;

	IMemAllocMgr *pMgr=g_Engine->GetMemAllocMgr();
	if(pMgr)
	{
		char szTemp[64];
		int iUsed,iFree;
		pMgr->GetAllocatorInfos(iSelected,szTemp,iUsed,iFree);

		sprintf(szTemp,"Utilisés: %i",iUsed);
		GetDlgItem(IDC_ALLOCUSED)->SetWindowText(szTemp);

		sprintf(szTemp,"Libres: %i",iFree);
		GetDlgItem(IDC_ALLOCFREE)->SetWindowText(szTemp);

		sprintf(szTemp,"Total: %i",iUsed + iFree);
		GetDlgItem(IDC_ALLOCTOTAL)->SetWindowText(szTemp);
	}
}

void CMemory::OnOK()
{
	ShowWindow(SW_HIDE);
}

void CMemory::OnCancel()
{
	ShowWindow(SW_HIDE);
}

BOOL CMemory::OnInitDialog()
{
	if(!CDialog::OnInitDialog())
		return FALSE;

	m_MemList.ShowWindow(SW_HIDE);

	m_Tab.InsertItem(0,"Allocateurs");
	m_Tab.InsertItem(1,"Classes suivies");
	m_Tab.SetCurSel(0);

	m_MemList.InsertColumn(0,"Classe",0,140);
	m_MemList.InsertColumn(1,"Taille",0,70);
	m_MemList.InsertColumn(2,"NbNew",0,50);
	m_MemList.InsertColumn(3,"NbDel",0,50);
	m_MemList.InsertColumn(4,"NbMNew",0,55);
	m_MemList.InsertColumn(5,"NbMDel",0,50);
	m_MemList.InsertColumn(6,"NbInst",0,50);
	m_MemList.InsertColumn(7,"TailleStd",0,70);
	m_MemList.InsertColumn(8,"NbExtra",0,55);
	m_MemList.InsertColumn(9,"TailleEx",0,70);
	m_MemList.InsertColumn(10,"Total",0,70);

	return TRUE;
}

void CMemory::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	if(bShow)
		OnBnClickedMemrefresh();
}

#endif