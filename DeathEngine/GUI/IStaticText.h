
#ifndef _ISTATICTEXT_H_

#define _ISTATICTEXT_H_

#include "IElement.h"
#include "../Core/Color.h"

namespace GUI
{

class IFont;

class IStaticText : public IElement
{
	public:

		IStaticText(IEnvironment *pEnvironment, IElement *pParent, IEventReceiver *pUser,
			uint32 dwID, Core::Rect rect) :
		  IElement(ET_STATICTEXT,pEnvironment,pParent,pUser,dwID,rect) {};
		virtual ~IStaticText() {};

		virtual void SetOverrideFont(IFont *pFont=0)=0;
		virtual IFont *GetOverrideFont() const=0;

		virtual void SetOverrideColor(Core::Color32 cColor)=0;
		virtual const Core::Color32 &GetOverrideColor() const=0;
		virtual void EnableOverrideColor(bool bEnable)=0;
		virtual bool IsOverrideColorEnabled() const=0;

		virtual void SetBackgroundColor(Core::Color32 cColor)=0;
		virtual void SetDrawBackground(bool bDraw)=0;

		virtual void SetDrawBorder(bool bDraw)=0;

		virtual void SetTextAlignment(ALIGNMENT Horizontal, ALIGNMENT Vertical)=0;

		virtual void SetWordWrap(bool bEnable)=0;
		virtual bool IsWordWrapEnabled() const=0;

		virtual int GetTextHeight() const=0;
		virtual int GetTextWidth() const=0;
};

}

#endif