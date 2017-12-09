
#ifndef _TABCONTROL_H_

#define _TABCONTROL_H_

#include "ITabControl.h"
#include <vector>
#include "../Dev/MemDebugger.h"

namespace GUI
{

class Tab : public ITab
{
	public:

		Tab(int iNumber, IEnvironment *pEnvironment, IElement *pParent, IEventReceiver *pUser,
			const Core::Rect& rect, uint32 dwID);

		virtual void Reset(int iNumber, IEnvironment *pEnvironment, IElement *pParent,
			IEventReceiver *pUser, const Core::Rect& rect, uint32 dwID);
		virtual void Clear();
		
		virtual int GetNumber() const;
		virtual void SetNumber(int n);

		virtual void Draw();

		virtual void SetDrawBackground(bool bDraw=true);
		virtual void SetBackgroundColor(Core::Color32 cColor);
		virtual bool IsDrawingBackground() const;
		virtual Core::Color32 GetBackgroundColor() const;
		virtual bool IsPointInside(const Core::Vector2i& p) const;

		MEMDEBUG_IMPL
		MEMDEBUG_CLASSNAME("Tab")
		MEMDEBUG_CLASSSIZE(sizeof(Tab))

	private:

		int m_iNumber;
		bool m_bDrawBackground;
		Core::Color32 m_cBackColor;
};

class TabControl : public ITabControl
{
	public:

		TabControl(IEnvironment *pEnvironment, IElement *pParent, IEventReceiver *pUser,
			const Core::Rect& rect, bool bFillBackground=true, bool bBorder=true,
			uint32 dwID=INVALID_ID);
		virtual ~TabControl();

		virtual void Reset(IEnvironment *pEnvironment, IElement *pParent, IEventReceiver *pUser,
			const Core::Rect& rect, bool bFillBackground=true, bool bBorder=true,
			uint32 dwID=INVALID_ID);
		virtual void Clear();

		virtual ITab *AddTab(const char *szCaption, int iID=-1);
		virtual void AddTab(Tab *pTab);
		virtual int GetTabCount() const;
		virtual ITab *GetTab(int iIndex) const;

		virtual bool SetActiveTab(int iIndex);
		virtual bool SetActiveTab(IElement *pTab);
		virtual int GetActiveTab() const;

		virtual bool OnEvent(const Event& _Event);
		virtual void Draw();

		virtual void RemoveChild(IElement *pChild, bool bDelete=false);
		virtual bool IsPointInside(const Core::Vector2i& p) const;

		MEMDEBUG_IMPL
		MEMDEBUG_CLASSNAME("TabControl")
		MEMDEBUG_CLASSSIZE(sizeof(TabControl))

	private:

		void SelectTab(Core::Vector2i p);

		std::vector<Tab *> m_Tabs;
		int m_iActiveTab;
		bool m_bBorder;
		bool m_bFillBackground;
};

}

#endif
