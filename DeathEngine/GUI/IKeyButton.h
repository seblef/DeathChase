
#ifndef _IKEYBUTTON_H_
#define _IKEYBUTTON_H_

#include "IElement.h"

namespace GUI
{

class IKeyButton : public IElement
{
	public:

		IKeyButton(IEnvironment *pEnvironment, IElement *pParent, IEventReceiver *pUser,
			uint32 dwID, Core::Rect rect) :
		  IElement(ET_KEYBUTTON,pEnvironment,pParent,pUser,dwID,rect) {};
	  virtual ~IKeyButton() {};

	  virtual byte GetKey() const=0;
	  virtual void SetKey(const byte byKey)=0;
};

}

#endif

