
#ifndef _EDITBOX_H_

#define _EDITBOX_H_

#include "IEditBox.h"
#include <vector>
#include "../Dev/MemDebugger.h"

namespace GUI
{

class EditBox : public IEditBox
{
	public:

		EditBox(const char *szText, bool bBorder, IEnvironment *pEnvironment,
			IElement *pParent, IEventReceiver *pUser, uint32 dwID, const Core::Rect& rect);
		virtual ~EditBox();

		virtual void Reset(const char *szText, bool bBorder, IEnvironment *pEnvironment,
			IElement *pParent, IEventReceiver *pUser, uint32 dwID, const Core::Rect& rect);
		virtual void Clear();

		virtual void SetOverrideFont(IFont *pFont=0);
		virtual void SetOverrideColor(Core::Color32 cColor);
		virtual void EnableOverrideColor(bool bEnable);

		virtual void SetDrawBorder(bool bBorder);

		virtual void SetWordWrap(bool bEnable);
		virtual bool IsWordWrapEnabled() const;

		virtual void SetMultiLine(bool bEnable);
		virtual bool IsMultiLineEnabled() const;

		virtual void SetAutoScroll(bool bEnable);
		virtual bool IsAutoScrollEnabled() const;

		virtual Core::Vector2i GetTextDimension();
		virtual void SetTextAlignment(ALIGNMENT Horizontal, ALIGNMENT Vertical);

		virtual bool OnEvent(const Event& _Event);
		virtual void Draw();

		virtual void SetText(const char *szText);
		
		virtual void SetMax(uint32 dwMax);
		virtual uint32 GetMax() const;

		virtual void SetPasswordBox(bool bPasswordBox, char cPasswordChar='*');
		virtual bool IsPasswordBox() const;

		virtual void UpdateAbsolutePosition();

		MEMDEBUG_IMPL
		MEMDEBUG_CLASSNAME("EditBox")
		MEMDEBUG_CLASSSIZE(sizeof(EditBox))

	protected:

		void BreakText();
		void SetTextRect(int iLine);
		int GetLineFromPos(int iPos);
		void InputChar(char c);
		void CalculateScrollPos();

		bool ProcessKey(const Event& e);
		bool ProcessMouse(const Event& e);
		int GetCursorPos(int x, int y);

		bool m_bMouseMarking;
		bool m_bBorder;
		bool m_bOverrideColorEnabled;
		int m_iMarkBegin;
		int m_iMarkEnd;

		Core::Color32 m_cOverrideColor;
		IFont *m_pOverrideFont, *m_pLastBreakFont;

		uint32 m_dwBlinkStartTime;
		int m_iCursorPos;
		int m_iHScrollPos,m_iVScrollPos;
		uint32 m_dwMax;

		bool m_bWordWrap, m_bMultiLine, m_bAutoScroll, m_bPasswordBox;
		char m_cPasswordChar;
		ALIGNMENT m_HAlign,m_VAlign;

		std::vector<std::string> m_BrokenText;
		std::vector<int> m_BrokenTextPositions;

		Core::Rect m_CurrentTextRect,m_FrameRect;
};

}

#endif
