
#ifndef _SPINBOX_H_

#define _SPINBOX_H_

#include "ISpinBox.h"
#include "../Dev/MemDebugger.h"

namespace GUI
{

class IEditBox;
class IButton;

class SpinBox : public ISpinBox
{
	public:

		SpinBox(const char *szText, IEnvironment *pEnvironment,
			IElement *pParent, IEventReceiver *pUser, uint32 dwID, const Core::Rect& rect);
		virtual ~SpinBox();

		virtual void Reset(const char *szText, IEnvironment *pEnvironment,
			IElement *pParent, IEventReceiver *pUser, uint32 dwID, const Core::Rect& rect);
		virtual void Clear();

		virtual IEditBox *GetEditBox_() const;

		virtual void SetValue(float val);
		virtual float GetValue() const;

		virtual void SetRange(float min, float max);
		virtual float GetMin() const;
		virtual float GetMax() const;

		virtual void SetStepSize(float step=1.0f);
		virtual float GetStepSize() const;

		virtual bool OnEvent(const Event& _Event);

		virtual void SetText(const char *szText);
		virtual const char *GetText() const;

		virtual void SetDecimalPlaces(int places);

		MEMDEBUG_IMPL
		MEMDEBUG_CLASSNAME("SpinBox")
		MEMDEBUG_CLASSSIZE(sizeof(SpinBox))

	protected:

		virtual void VerifyValueRange(float &val);

		IEditBox *m_pEditBox;
		IButton *m_pSpinUp;
		IButton *m_pSpinDown;
		float m_fStepSize;
		float m_fRangeMin;
		float m_fRangeMax;

		std::string m_strFormat;
		int m_iDecimalPlaces;
};

}

#endif
