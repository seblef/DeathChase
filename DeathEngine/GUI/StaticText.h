
#ifndef _STATICTEXT_H_

#define _STATICTEXT_H_

#include "IStaticText.h"
#include <vector>
#include "../Dev/MemDebugger.h"

namespace GUI
{

class StaticText : public IStaticText
{
	public:

		StaticText(const char *szText, bool bBorder, IEnvironment *pEnvironment,
			IElement *pParent, IEventReceiver *pUser, 
			uint32 dwID, const Core::Rect& rect, bool bBackground=false);
		virtual ~StaticText();

		virtual void Reset(const char *szText, bool bBorder, IEnvironment *pEnvironment,
			IElement *pParent, IEventReceiver *pUser,
			uint32 dwID, const Core::Rect& rect, bool bBackground=false);
		virtual void Clear();

		virtual void Draw();

		virtual void SetOverrideFont(IFont *pFont=0);
		virtual IFont *GetOverrideFont() const;

		virtual void SetOverrideColor(Core::Color32 cColor);
		virtual const Core::Color32& GetOverrideColor() const;
		virtual void EnableOverrideColor(bool bEnable);
		virtual bool IsOverrideColorEnabled() const;

		virtual void SetBackgroundColor(Core::Color32 cColor);
		virtual void SetDrawBackground(bool bDraw);

		virtual void SetDrawBorder(bool bDraw);

		virtual void SetTextAlignment(ALIGNMENT Horizontal, ALIGNMENT Vertical);

		virtual void SetWordWrap(bool bEnable);
		virtual bool IsWordWrapEnabled() const;

		virtual void SetText(const char *szText);

		virtual int GetTextHeight() const;
		virtual int GetTextWidth() const;

		virtual void UpdateAbsolutePosition();

		MEMDEBUG_IMPL
		MEMDEBUG_CLASSNAME("StaticText")
		MEMDEBUG_CLASSSIZE(sizeof(StaticText))

	private:

		void BreakText();

		bool m_bBorder;
		ALIGNMENT m_HAlign,m_VAlign;
		bool m_bOverrideColor;
		bool m_bWordWrap;
		bool m_bBackground;

		Core::Color32 m_cOverrideColor,m_cBGColor;
		IFont *m_pOverrideFont,*m_pLastBreakFont;

		std::vector<std::string> m_BrokenText;
};

}

#endif
