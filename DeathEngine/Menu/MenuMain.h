
#ifndef _MENUMAIN_H_
#define _MENUMAIN_H_

#include "IScreenMenu.h"

namespace Menu
{

class MenuMain : public IScreenMenu
{
	public:

		MenuMain();
		~MenuMain();

		bool Start();
		bool End();

		bool Update();

		MenuType GetType() { return MENU_MAIN; }
		const char *GetName() { return "Death Chase"; }

	private:

		int m_iSelected;

};

}

#endif
