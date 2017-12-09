
#ifndef _IEVENTRECEIVER_H_

#define _IEVENTRECEIVER_H_

#include "../Common.h"

namespace GUI
{

enum EVENTTYPE
{
	ET_GUI=0,
	ET_MOUSE,
	ET_KEY,
	ET_USER

};

enum MOUSEEVENT
{
	ME_LMOUSEDOWN=0,
	ME_RMOUSEDOWN,
	ME_MMOUSEDOWN,
	ME_LMOUSEUP,
	ME_RMOUSEUP,
	ME_MMOUSEUP,
	ME_MOVED,
	ME_MOUSEWHEEL,

	ME_COUNT
};

class IElement;

enum GUIEVENT
{
	GE_FOCUSLOST=0,
	GE_FOCUSED,
	GE_HOVERED,
	GE_LEFT,
	GE_CLOSED,
	GE_BUTTONCLICKED,
	GE_SCROLLBARCHANGED,
	GE_CHECKBOXCHANGED,
	GE_LISTBOXCHANGED,
	GE_LISTBOXSELECTEDAGAIN,
	GE_FILESELECTED,
	GE_FILECANCELLED,
	GE_MSGBOXYES,
	GE_MSGBOXNO,
	GE_MSGBOXOK,
	GE_MSGBOXCANCEL,
	GE_EDITBOXENTER,
	GE_TABCHANGED,
	GE_MENUSELECTED,
	GE_COMBOBOXCHANGED,
	GE_SPINBOXCHANGED,
	GE_MOVED,
	GE_MINIMIZE,
	GE_RESTORE,
	GE_KEYSELECTED

};

struct Event
{
	struct GUIEvent
	{
		IElement *pCaller;
		IElement *pElement;
		GUIEVENT EventType;

	};

	struct MouseInput
	{
		int dwX;
		int dwY;
		float fWheel;
		MOUSEEVENT EventType;

	};

	struct KeyInput
	{
		char cChar;
		byte cDXChar;
		bool bPressed;
		bool bShift;
		bool bControl;

	};

	struct UserEvent
	{
		uint32 dwParam1;
		uint32 dwParam2;
		float fParam;
		void *pParam;

	};

	EVENTTYPE EventType;

	union
	{
		struct GUIEvent sGUI;
		struct MouseInput sMouse;
		struct KeyInput sKey;
		struct UserEvent sUser;

	};
};

class IEventReceiver
{
	public:

		virtual ~IEventReceiver() {}
		virtual bool OnEvent(const Event& _Event)=0;
};

}

#endif

