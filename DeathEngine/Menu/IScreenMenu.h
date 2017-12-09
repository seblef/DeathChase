
#ifndef _ISCREENMENU_H_
#define _ISCREENMENU_H_

namespace Menu
{

#define MENU_DISTANCEY				30

enum MenuType
{
	MENU_MAIN=0,
	MENU_INGAME,
	MENU_SOLO,
	MENU_MULTI,
	MENU_OPTIONS,

	MENU_COUNT

};

class IScreenMenu
{
	public:

		IScreenMenu() {}
		~IScreenMenu() {}

		virtual bool Start()=0;
		virtual bool End()=0;

		virtual bool Update()=0;
		virtual MenuType GetType()=0;
		virtual const char *GetName()=0;

};

}

#endif