
#ifdef DEDEV
#include "DevTask.h"
#include "Console.h"
#include "MemoryApp.h"
#include "../GUI/IContextMenu.h"
#include "../GUI/IEnvironment.h"
#include "../GUI/GUITask.h"

#define ID_DEVMENU				12000
#define ID_DEVMENUCONSOLE		12001
#define ID_DEVMENUMEMORY		12002

namespace Dev
{

DevTask::DevTask(const int iPriority) : ITask(iPriority),
	m_pConsole(0)
{
	strcpy(m_szName,"Dev");
}

DevTask::~DevTask()
{
	Stop();
}

bool DevTask::Initialize()
{
	GUI::IContextMenu *pMenu=g_GUI.GetMainMenu();
	g_GUI.AddMenuEventReceiver(this);
	g_GUI.ShowMenu(true);

	uint32 dwDevMenu=pMenu->AddItem("Dev",INVALID_ID,true,true);
	pMenu=pMenu->GetSubMenu(dwDevMenu);

	uint32 dwItem=pMenu->AddItem("Console",ID_DEVMENUCONSOLE,true,false,true);
	pMenu->SetItemChecked(dwItem,false);
	dwItem=pMenu->AddItem("Memoire",ID_DEVMENUMEMORY,true,false,true);
	pMenu->SetItemChecked(dwItem,false);

	m_pConsole=new ConsoleApp;
	m_pConsole->LoadTemplate("console.tdlg");
	m_pConsole->Show(false);

	m_pMemory=new MemoryApp;
	m_pMemory->LoadTemplate("memory.tdlg");
	m_pMemory->Show(false);

	return true;
}

bool DevTask::Stop()
{
	if(m_pConsole) delete m_pConsole;
	m_pConsole=0;

	if(m_pMemory) delete m_pMemory;
	m_pMemory=0;

	return true;
}

bool DevTask::OnSuspend()
{
	return true;
}

bool DevTask::OnResume()
{
	return true;
}

bool DevTask::Update()
{
	return true;
}

bool DevTask::OnEvent(const GUI::Event &_Event)
{
	if(_Event.EventType==GUI::ET_GUI && _Event.sGUI.EventType==GUI::GE_MENUSELECTED)
	{
		GUI::IContextMenu *pMenu=(GUI::IContextMenu *)_Event.sGUI.pCaller;
		int iSelected=pMenu->GetSelectedItem();

		if(iSelected!=-1)
		{
			bool bItemChecked=pMenu->IsItemChecked((uint32)iSelected);

			if(pMenu->GetItemCommandID(iSelected)==ID_DEVMENUCONSOLE)
			{
				m_pConsole->Show(!bItemChecked);
				pMenu->SetItemChecked((uint32)iSelected,!bItemChecked);
			}
			else if(pMenu->GetItemCommandID(iSelected)==ID_DEVMENUMEMORY)
			{
				m_pMemory->Show(!bItemChecked);
				pMenu->SetItemChecked((uint32)iSelected,!bItemChecked);
			}
		}
	}

	return true;
}

}

#endif