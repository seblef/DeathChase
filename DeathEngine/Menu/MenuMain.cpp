
#include "MenuMain.h"
#include "MenuTask.h"
#include "../GUI/IFont.h"
#include "../Core/Renderer2D.h"
#include "../Core/Renderer.h"
#include "../Core/Input.h"
#include "../Core/Kernel.h"

namespace Menu
{

enum MainMenuSelection
{
	MMS_SOLO=0,
	MMS_MULTI,
	MMS_OPTIONS,
	MMS_QUIT,

	MMS_COUNT
};

struct MainMenuItem
{
	MainMenuSelection Selection;
	char szText[64];
	int Command;

};

MainMenuItem MMItems[MMS_COUNT]={
	{ MMS_SOLO, "Solo", MENU_SOLO },
	{ MMS_MULTI, "Multijoueurs", MENU_MULTI },
	{ MMS_OPTIONS, "Options", MENU_OPTIONS },
	{ MMS_QUIT, "Quitter", -1 } };

MenuMain::MenuMain() : m_iSelected(0)
{
}

MenuMain::~MenuMain()
{
}

bool MenuMain::Start()
{
	m_iSelected=0;
	return true;
}

bool MenuMain::End()
{
	return true;
}

bool MenuMain::Update()
{
	int i;
	Core::Vector2i text_dim;
	int height;
	int text_height;
	GUI::IFont *font=g_Menu.GetMenuSmallFont();
	Core::Rect text_rect;
	
	if(!font) return false;

	text_height=font->GetDimension("A").y;
	height=text_height * MMS_COUNT + MENU_DISTANCEY * (MMS_COUNT-1);
	text_rect.m_vUpperLeft.y=(g_Renderer.GetViewportSize().y - text_height - MENU_DISTANCEY * 2)/ 2 - height / 2;

	for(i=0;i<MMS_COUNT;i++)
	{
		text_dim=font->GetDimension(MMItems[i].szText);
		text_rect.m_vUpperLeft.x=(g_Renderer.GetViewportSize().x - text_dim.x) / 2;
		text_rect.m_vLowerRight=text_rect.m_vUpperLeft + text_dim;

		font->Draw(MMItems[i].szText,text_rect,
			m_iSelected==i ? g_Menu.GetSelectedColor() : g_Menu.GetNormalColor());

		text_rect.m_vUpperLeft.y+=text_height + MENU_DISTANCEY;
	}

	Core::Input::KeyboardState kstate;
	g_Input.GetKeyboardState(kstate);

	if((kstate.cKeys[DIK_UP] & 0x80) && !(kstate.cLastKeys[DIK_UP] & 0x80))
	{
		m_iSelected--;
		if(m_iSelected < 0) m_iSelected=MMS_COUNT-1;
	}
	else if((kstate.cKeys[DIK_DOWN] & 0x80) && !(kstate.cLastKeys[DIK_DOWN] & 0x80))
	{
		m_iSelected++;
		if(m_iSelected >= MMS_COUNT) m_iSelected=0;
	}
	else if((kstate.cKeys[DIK_RETURN] & 0x80) && !(kstate.cLastKeys[DIK_RETURN] & 0x80))
	{
		switch(m_iSelected)
		{
			case MMS_QUIT:
				g_Kernel.KillAllTasks();
				break;

			case MMS_OPTIONS:
				g_Menu.SetMenu((MenuType)MMItems[MMS_OPTIONS].Command);
				break;

			case MMS_SOLO:
				g_Menu.SetMenu((MenuType)MMItems[MMS_SOLO].Command);
				break;

			default:
				break;
		}
	}

	return true;
}

}