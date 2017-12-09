
#ifndef _ICOLORDIALOG_H_

#define _ICOLORDIALOG_H_

#include "IElement.h"

namespace GUI
{

class IColorDialog : public IElement
{
	public:

		IColorDialog(IEnvironment *pEnvironment, IElement *pParent, IEventReceiver *pUser,
			uint32 dwID, Core::Rect rect) :
		  IElement(ET_COLORDIALOG,pEnvironment,pParent,pUser,dwID,rect) {};
		virtual ~IColorDialog() {};
};

}

#endif
