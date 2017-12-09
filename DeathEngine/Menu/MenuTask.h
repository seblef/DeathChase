
#ifndef _MENUTASK_H_
#define _MENUTASK_H_

#include "../Core/TSingleton.h"
#include "../Core/ITask.h"
#include "IScreenMenu.h"
#include "../Common.h"
#include "../Core/Color.h"

namespace GUI
{

class IFont;

}

namespace Menu
{

class MenuTask : public Core::ITask, public Core::TSingleton<MenuTask>
{
	public:

		MenuTask(int iPriority);
		~MenuTask();

		bool Initialize();
		bool Stop();

		bool OnSuspend();
		bool OnResume();

		bool Update();

		bool SetMenu(MenuType _Type);
		bool SetMenu(IScreenMenu *pMenu);
		bool SetPrecMenu();

		GUI::IFont *GetMenuSmallFont() { return m_pMenuSmallFont; }
		bool SetMenuSmallFont(GUI::IFont *pFont);
		bool SetMenuSmallFont(const char *szName);

		GUI::IFont *GetMenuLargeFont() { return m_pMenuLargeFont; }
		bool SetMenuLargeFont(GUI::IFont *pFont);
		bool SetMenuLargeFont(const char *szName);

		Core::Color32 GetNormalColor() const { return m_cNormalColor; }
		void SetNormalColor(Core::Color32 cColor) { m_cNormalColor=cColor; }

		Core::Color32 GetSelectedColor() const { return m_cSelectedColor; }
		void SetSelectedColor(Core::Color32 cColor) { m_cSelectedColor=cColor; }

	private:

		IScreenMenu *m_pCurrentMenu;
		IScreenMenu *m_pPrecMenu;
		GUI::IFont *m_pMenuSmallFont;
		GUI::IFont *m_pMenuLargeFont;

		Core::Color32 m_cSelectedColor;
		Core::Color32 m_cNormalColor;

};

}

#define g_Menu				Menu::MenuTask::GetSingletonRef()

#endif
