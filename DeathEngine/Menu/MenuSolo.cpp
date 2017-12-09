
#include "MenuSolo.h"
#include "MenuTask.h"
#include "../GUI/GUITask.h"
#include "../GUI/IEnvironment.h"
#include "../GUI/FileDialog.h"
#include "../Game/GameState.h"
#include "../Core/Kernel.h"

#define MENUSOLO_MAPFILEDIALOG				1100
#define MENUSOLO_SPACESHIPFILEDIALOG		1101

namespace Menu
{

MenuSolo::MenuSolo()
{
}

MenuSolo::~MenuSolo()
{
}

bool MenuSolo::Start()
{
	GUI::FileDialog *pDlg=(GUI::FileDialog *)g_GUI.GetEnvironment()->AddFileDialog("Map",true,false,0,this,MENUSOLO_MAPFILEDIALOG);
	pDlg->AddExtFilter(".dmap");

	return true;
}

bool MenuSolo::End()
{
	return true;
}

bool MenuSolo::Update()
{
	return true;
}

bool MenuSolo::OnEvent(const GUI::Event &Event)
{
	if(Event.EventType==GUI::ET_GUI)
	{
		if(Event.sGUI.EventType==GUI::GE_FILECANCELLED ||
			Event.sGUI.EventType==GUI::GE_FILESELECTED)
		{
			if(Event.sGUI.pCaller->GetID()==MENUSOLO_MAPFILEDIALOG)
			{
				if(Event.sGUI.EventType==GUI::GE_FILECANCELLED)
					g_Menu.SetMenu(MENU_MAIN);
				else
				{
					GUI::IFileDialog *pDlg=(GUI::IFileDialog *)Event.sGUI.pCaller;
					StartGame(pDlg->GetFileName(),pDlg->GetPath());
				}
			}
		}
	}

	return true;
}

void MenuSolo::StartGame(const char *szMapFileName,
						 const char *szMapPath)
{
	g_Menu.SetMenu(MENU_MAIN);

	if(g_Game.Load(szMapFileName,szMapPath,"F16.dship","Ships\\F16\\"))
	{
		g_Renderer.ClearRenderingBuffers();
		g_Kernel.RemoveTask(MenuTask::GetSingletonPtr());
		g_Kernel.AddTask(Game::GameState::GetSingletonPtr());
	}
}

}