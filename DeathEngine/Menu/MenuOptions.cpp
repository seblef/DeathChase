
#include "MenuOptions.h"
#include "MenuOptionsDlg.h"
#include "MenuTask.h"

namespace Menu
{

MenuOptions::MenuOptions() : m_pDlg(0)
{
}

MenuOptions::~MenuOptions()
{
	if(m_pDlg)
	{
		m_pDlg->Destroy();
		delete m_pDlg;
	}
}

bool MenuOptions::Start()
{
	if(m_pDlg)
		End();

	m_pDlg=new MenuOptionsDlg;
	return m_pDlg->LoadTemplate("options.tdlg");

	return true;
}

bool MenuOptions::End()
{
	if(m_pDlg)
	{
		m_pDlg->Destroy();
		delete m_pDlg;
	}

	m_pDlg=0;

	return true;
}

bool MenuOptions::Update()
{
	if(m_pDlg && m_pDlg->IsFinished())
		g_Menu.SetMenu(MENU_MAIN);

	return true;
}

}