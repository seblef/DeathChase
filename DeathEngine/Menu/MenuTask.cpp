
#include "MenuTask.h"
#include "../GUI/Font.h"
#include "../Core/Renderer2D.h"
#include "../Core/Renderer.h"
#include <string.h>

#include "MenuMain.h"
#include "MenuOptions.h"
#include "MenuSolo.h"

namespace Menu
{

MenuTask::MenuTask(int iPriority) : ITask(iPriority),
	m_pCurrentMenu(0),
	m_pPrecMenu(0),
	m_pMenuSmallFont(0),
	m_pMenuLargeFont(0),
	m_cNormalColor(210,200,200,200),
	m_cSelectedColor(210,240,20,20)
{
	strcpy(m_szName,"MenuTask");
}

MenuTask::~MenuTask()
{
	if(m_pCurrentMenu)
	{
		m_pCurrentMenu->End();
		delete m_pCurrentMenu;
		m_pCurrentMenu=0;
	}

	if(m_pPrecMenu)
	{
		m_pPrecMenu->End();
		delete m_pPrecMenu;
		m_pPrecMenu=0;
	}

	if(m_pMenuLargeFont)
		delete m_pMenuLargeFont;

	if(m_pMenuSmallFont)
		delete m_pMenuSmallFont;

	m_pMenuLargeFont=0;
	m_pMenuSmallFont=0;
}

bool MenuTask::Initialize()
{
	m_pMenuSmallFont=new GUI::Font(0,"lucidasmall.bmp");
	m_pMenuLargeFont=new GUI::Font(0,"lucidalarge.bmp");

	((GUI::Font *)m_pMenuSmallFont)->Load("lucidasmall.bmp");
	((GUI::Font *)m_pMenuLargeFont)->Load("lucidalarge.bmp");

	g_Renderer.EnableClearingColorBuffer(true);

	return true;
}

bool MenuTask::Stop()
{
	g_Renderer.EnableClearingColorBuffer(false);

	if(m_pCurrentMenu)
	{
		m_pCurrentMenu->End();
		delete m_pCurrentMenu;
		m_pCurrentMenu=0;
	}

	if(m_pMenuLargeFont)
		delete m_pMenuLargeFont;

	if(m_pMenuSmallFont)
		delete m_pMenuSmallFont;

	m_pMenuLargeFont=0;
	m_pMenuSmallFont=0;

	return true;
}

bool MenuTask::OnSuspend()
{
	g_Renderer.EnableClearingColorBuffer(false);

	return true;
}

bool MenuTask::OnResume()
{
	g_Renderer.EnableClearingColorBuffer(true);

	return true;
}

bool MenuTask::Update()
{
	if(!m_pCurrentMenu) return true;

	g_Renderer2D.Start();

	if(m_pMenuLargeFont)
	{
		Core::Vector2i text_dim(m_pMenuLargeFont->GetDimension(m_pCurrentMenu->GetName()));
		Core::Rect text_rect;
		text_rect.m_vUpperLeft.x=(g_Renderer.GetViewportSize().x - text_dim.x) / 2;
		text_rect.m_vUpperLeft.y=2 * MENU_DISTANCEY;
		text_rect.m_vLowerRight=text_rect.m_vUpperLeft + text_dim;

		m_pMenuLargeFont->Draw(m_pCurrentMenu->GetName(),text_rect,m_cSelectedColor);
	}

	ASSERTRETURN(m_pCurrentMenu->Update(),true,"");

	g_Renderer2D.End();

	return true;
}

bool MenuTask::SetMenu(IScreenMenu *pMenu)
{
	if(m_pCurrentMenu)
	{
		m_pCurrentMenu->End();

		if(m_pPrecMenu)
			delete m_pPrecMenu;

		m_pPrecMenu=m_pCurrentMenu;
		m_pCurrentMenu=pMenu;
	}

	m_pCurrentMenu=pMenu;
	return m_pCurrentMenu->Start();
}

bool MenuTask::SetMenu(MenuType _Type)
{
	switch(_Type)
	{
		case MENU_MAIN:
			return SetMenu(new MenuMain);
			break;

		case MENU_OPTIONS:
			return SetMenu(new MenuOptions);
			break;

		case MENU_SOLO:
			return SetMenu(new MenuSolo);
			break;

		default:
			return false;
	}

	return true;
}

bool MenuTask::SetPrecMenu()
{
	if(!m_pPrecMenu)
		return true;

	if(m_pCurrentMenu)
	{
		m_pCurrentMenu->End();
		delete m_pCurrentMenu;
	}

	m_pCurrentMenu=m_pPrecMenu;
	m_pPrecMenu=0;

	return m_pCurrentMenu->Start();
}


bool MenuTask::SetMenuLargeFont(const char *szName)
{
	return true;
}

bool MenuTask::SetMenuLargeFont(GUI::IFont *pFont)
{
	return true;
}

bool MenuTask::SetMenuSmallFont(const char *szName)
{
	return true;
}

bool MenuTask::SetMenuSmallFont(GUI::IFont *pFont)
{
	return true;
}

}