
#ifndef _ISCROLLBAR_H_

#define _ISCROLLBAR_H_

#include "IElement.h"

namespace GUI
{

class IScrollBar : public IElement
{	
	public:

		IScrollBar(IEnvironment *pEnvironment, IElement *pParent, IEventReceiver *pUser,
			uint32 dwID, Core::Rect rect) :
		  IElement(ET_SCROLLBAR,pEnvironment,pParent,pUser,dwID,rect) {};
		~IScrollBar() {};

		virtual int GetMax() const=0;
		virtual void SetMax(int iMax)=0;

		virtual int GetSmallStep() const=0;
		virtual void SetSmallStep(int iStep)=0;

		virtual int GetLargeStep() const=0;
		virtual void SetLargeStep(int iStep)=0;

		virtual int GetPos() const=0;
		virtual void SetPos(int iPos)=0;
};

}

#endif