
#ifndef _IWINDOW_H_

#define _IWINDOW_H_

#include "IElement.h"

namespace GUI
{

class IButton;

class IWindow : public IElement
{
	public:

		IWindow(IEnvironment *pEnvironment, IElement *pParent,IEventReceiver *pUser,
			uint32 dwID, Core::Rect rect) :
		  IElement(ET_WINDOW,pEnvironment,pParent,pUser,dwID,rect) {};
		virtual ~IWindow() {};

		virtual IButton *GetCloseButton() const=0;
		virtual IButton *GetMinimizeButton() const=0;
		virtual IButton *GetMaximizeButton() const=0;
};

}

#endif
