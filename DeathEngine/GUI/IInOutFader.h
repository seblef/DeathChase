
#ifndef _IINOUTFADER_H_

#define _IINOUTFADER_H_

#include "IElement.h"
#include "../Core/Color.h"

namespace GUI
{

class IInOutFader : public IElement
{
	public:

		IInOutFader(IEnvironment *pEnvironment, IElement *pParent, IEventReceiver *pUser,
			uint32 dwID, Core::Rect rect) :
		  IElement(ET_INOUTFADER,pEnvironment,pParent,pUser,dwID,rect) {};
		virtual ~IInOutFader() {};

		virtual Core::Color32 GetColor() const=0;

		virtual void SetColor(Core::Color32 cColor)=0;
		virtual void SetColor(Core::Color32 cSource, Core::Color32 cDest)=0;

		virtual void FadeIn(uint32 dwTime)=0;
		virtual void FadeOut(uint32 dwTime)=0;

		virtual bool IsReady() const=0;
};

}

#endif
