
#ifndef _MENUOPTIONS_H_
#define _MENUOPTIONS_H_

#include "IScreenMenu.h"

namespace Menu
{

class MenuOptionsDlg;

class MenuOptions : public IScreenMenu
{
	public:

		MenuOptions();
		~MenuOptions();

		bool Start();
		bool End();

		bool Update();

		MenuType GetType() { return MENU_OPTIONS; }
		const char *GetName() { return "Options"; }

	private:

		MenuOptionsDlg *m_pDlg;

};

}

#endif