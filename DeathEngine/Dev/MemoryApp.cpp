
#ifdef DEDEV

#include "MemoryApp.h"
#include "MemAllocMgr.h"
#include "MemDebugger.h"

#include "../GUI/IButton.h"
#include "../GUI/IListBox.h"
#include "../GUI/IStaticText.h"
#include "../GUI/ITabControl.h"
#include "../GUI/MultiListBox.h"
#include "../GUI/GUITask.h"

#define ID_MEMTAB				201
#define ID_MEMREFRESH			202
#define ID_MEMALLOC_ALLOCS		203
#define ID_MEMALLOC_USED		204
#define ID_MEMALLOC_FREE		205
#define ID_MEMALLOC_TOTAL		206
#define ID_MEMDEBUG				299


namespace Dev
{

MemoryApp::MemoryApp() :
	m_pTab(0),
	m_pRefresh(0),
	m_pAllocatorsList(0),
	m_pAllocFree(0),
	m_pAllocUsed(0),
	m_pAllocTotal(0),
	m_pMemDebug(0)
{
}

MemoryApp::~MemoryApp()
{
	if(m_pMemDebug)
	{
		RemoveControl((GUI::IElement *)m_pMemDebug);
		delete m_pMemDebug;
	}

	m_pMemDebug=0;
}

void MemoryApp::OnCreate()
{
	m_pTab=(GUI::ITabControl *)GetElementByID(ID_MEMTAB);
	m_pTab->SetActiveTab(0);

	m_pRefresh=(GUI::IButton *)GetElementByID(ID_MEMREFRESH);
	m_pAllocatorsList=(GUI::IListBox *)GetElementByID(ID_MEMALLOC_ALLOCS);
	m_pAllocFree=(GUI::IStaticText *)GetElementByID(ID_MEMALLOC_FREE);
	m_pAllocUsed=(GUI::IStaticText *)GetElementByID(ID_MEMALLOC_USED);
	m_pAllocTotal=(GUI::IStaticText *)GetElementByID(ID_MEMALLOC_TOTAL);
	
	int sz[11]={110,50,40,40,40,40,40,60,40,60,60};
	m_pMemDebug=new GUI::MultiListBox(11,sz,g_GUI.GetEnvironment(),
		(GUI::IElement *)m_pWindow,this,ID_MEMDEBUG,Core::Rect(10,45,590,365));
	m_pMemDebug->SetVisible(false);
	AddControl((GUI::IElement *)m_pMemDebug);

	RefreshAllocators();
	RefreshMemDebug();
}

void MemoryApp::OnClick(const uint32 dwID)
{
	if(dwID==ID_MEMREFRESH)
	{
		RefreshAllocators();
		RefreshMemDebug();
	}
}

void MemoryApp::OnTab(const uint32 dwID)
{
	if(dwID==ID_MEMTAB)
	{
		int iIndex=m_pTab->GetActiveTab();

		switch(iIndex)
		{
			case 0:
				ShowAllocators(true);
				m_pMemDebug->SetVisible(false);
				break;

			case 1:
				ShowAllocators(false);
				m_pMemDebug->SetVisible(true);
				break;
		}
	}
}

void MemoryApp::OnSelect(const uint32 dwID)
{
	if(dwID==ID_MEMALLOC_ALLOCS)
	{
		int iSelected=m_pAllocatorsList->GetSelected();

		if(iSelected >= 0 && iSelected < g_MemAllocMgr.GetAllocatorCount())
		{
			MemAllocVector& v=g_MemAllocMgr.GetAllocators();
			char szText[64];

			sprintf(szText,"Utilises: %i",v[iSelected]->GetUsedItemsCount());
			m_pAllocUsed->SetText(szText);

			sprintf(szText,"Libres: %i",v[iSelected]->GetFreeCount());
			m_pAllocFree->SetText(szText);

			sprintf(szText,"Total: %i",v[iSelected]->GetItemCount());
			m_pAllocTotal->SetText(szText);
		}
		else
		{
			m_pAllocUsed->SetText("Utilises:");
			m_pAllocFree->SetText("Libres:");
			m_pAllocTotal->SetText("Total:");
		}
	}
}

void MemoryApp::RefreshAllocators()
{
	MemAllocVector& v=g_MemAllocMgr.GetAllocators();
	MemAllocVector::iterator i;

	int iSelected=m_pAllocatorsList->GetSelected();
	m_pAllocatorsList->ClearList();

	for(i=v.begin();i!=v.end();i++)
		m_pAllocatorsList->AddItem((*i)->GetClassName_());

	m_pAllocatorsList->SetSelected(iSelected);
}

void MemoryApp::ShowAllocators(bool bShow)
{
	m_pAllocatorsList->SetVisible(bShow);
	m_pAllocUsed->SetVisible(bShow);
	m_pAllocFree->SetVisible(bShow);
	m_pAllocTotal->SetVisible(bShow);
}

void MemoryApp::RefreshMemDebug()
{
	int total_new=0,total_del=0;
	int total_mnew=0,total_mdel=0;
	int total_size=0,total_extrasize=0;
	int total_inst=0,total_extra=0;
	int total_total=0,total_sizes=0;

	m_pMemDebug->ClearList();

	std::vector<std::string> line;
	std::string temp="Nom de classe";
	line.push_back(temp);

	temp="Taille"; line.push_back(temp);
	temp="NbNew"; line.push_back(temp);
	temp="NbDel"; line.push_back(temp);
	temp="NbMNew"; line.push_back(temp);
	temp="NbMDel"; line.push_back(temp);
	temp="NbInst"; line.push_back(temp);
	temp="TailleS"; line.push_back(temp);
	temp="NbEx"; line.push_back(temp);
	temp="TailleE"; line.push_back(temp);
	temp="Total"; line.push_back(temp);

	m_pMemDebug->AddItem(&line);

	const MemDebuggerVector& items=g_MemDebugger.GetItems();
	MemDebuggerVector::const_iterator i;

	for(i=items.begin();i!=items.end();i++)
	{
		line.clear();
		temp=i->szName; line.push_back(temp);
		temp=FormatSize(i->iSize); line.push_back(temp);
		temp=ToChar(i->iNewCount); line.push_back(temp);
		temp=ToChar(i->iDelCount); line.push_back(temp);
		temp=ToChar(i->iMNewCount); line.push_back(temp);
		temp=ToChar(i->iMDelCount); line.push_back(temp);
		temp=ToChar(i->iInstances); line.push_back(temp);
		temp=FormatSize(i->iInstances * i->iSize); line.push_back(temp);
		temp=ToChar(i->iExtraCount); line.push_back(temp);
		temp=FormatSize(i->iExtraSize); line.push_back(temp);
		temp=FormatSize(i->iTotalSize); line.push_back(temp);
		m_pMemDebug->AddItem(&line);

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

	line.clear();
	temp="Total"; line.push_back(temp);
	temp=FormatSize(total_size); line.push_back(temp);
	temp=ToChar(total_new); line.push_back(temp);
	temp=ToChar(total_del); line.push_back(temp);
	temp=ToChar(total_mnew); line.push_back(temp);
	temp=ToChar(total_mdel); line.push_back(temp);
	temp=ToChar(total_inst); line.push_back(temp);
	temp=FormatSize(total_sizes); line.push_back(temp);
	temp=ToChar(total_extra); line.push_back(temp);
	temp=FormatSize(total_extrasize); line.push_back(temp);
	temp=FormatSize(total_total); line.push_back(temp);
	m_pMemDebug->AddItem(&line);
}

const char *MemoryApp::FormatSize(int iSize)
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

const char *MemoryApp::ToChar(int i)
{
	static char szTemp[32];
	sprintf(szTemp,"%i",i);
	return szTemp;
}

}

#endif