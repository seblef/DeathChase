
#ifndef _ICHECKBOX_H_

#define _ICHECKBOX_H_

#include "IElement.h"

namespace GUI
{

class ICheckBox : public IElement
{
	public:

		ICheckBox(IEnvironment *pEnvironment, IElement *pParent, IEventReceiver *pUser,
			uint32 dwID, Core::Rect rect) :
		  IElement(ET_CHECKBOX,pEnvironment,pParent,pUser,dwID,rect) {};
		virtual ~ICheckBox() {};

		virtual void SetChecked(bool bChecked)=0;
		virtual bool IsChecked() const=0;
};

}

#endif
