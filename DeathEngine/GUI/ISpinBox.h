
#ifndef _ISPINBOX_H_

#define _ISPINBOX_H_

#include "IElement.h"

namespace GUI
{

class IEditBox;

class ISpinBox : public IElement
{
	public:

		ISpinBox(IEnvironment *pEnvironment, IElement *pParent, IEventReceiver *pUser,
			uint32 dwID, Core::Rect rect) :
			IElement(ET_SPINBOX,pEnvironment,pParent,pUser,dwID,rect) {}
		virtual ~ISpinBox() {}

		virtual IEditBox *GetEditBox_() const=0;

		virtual void SetValue(float fVal)=0;
		virtual float GetValue() const=0;

		virtual void SetRange(float min, float max)=0;
		virtual float GetMin() const=0;
		virtual float GetMax() const=0;

		virtual void SetStepSize(float step=1.0f)=0;
		virtual float GetStepSize() const=0;

		virtual void SetDecimalPlaces(int places)=0;
};

}

#endif