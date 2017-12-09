
#ifndef _MENUSOLO_H_
#define _MENUSOLO_H_

#include "IScreenMenu.h"
#include "../GUI/IEventReceiver.h"

namespace Menu
{

class MenuSolo : public IScreenMenu, public GUI::IEventReceiver
{
	public:

		MenuSolo();
		~MenuSolo();

		bool Start();
		bool End();

		bool Update();

		MenuType GetType() { return MENU_SOLO; }
		const char *GetName() { return "Solo"; }

		bool OnEvent(const GUI::Event& Event);

	protected:

		void StartGame(const char *szMapFileName, const char *szMapPath);

};

}

#endif