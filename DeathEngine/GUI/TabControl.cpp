
#include "TabControl.h"
#include "ISkin.h"
#include "IEnvironment.h"
#include "IFont.h"

namespace GUI
{

Tab::Tab(int iNumber, IEnvironment *pEnvironment, IElement *pParent, IEventReceiver *pUser,
		 const Core::Rect& rect, uint32 dwID) :
	ITab(pEnvironment,pParent,pUser,dwID,rect), m_iNumber(iNumber),
		m_bDrawBackground(false),m_cBackColor(0,0,0,0)
{
}

void Tab::Reset(int iNumber, IEnvironment *pEnvironment, IElement *pParent, IEventReceiver *pUser,
		 const Core::Rect& rect, uint32 dwID)
{
	IElement::Reset(pEnvironment,pParent,pUser,dwID,rect);

	m_iNumber=iNumber;
}

void Tab::Clear()
{
	m_bDrawBackground=false;
	m_cBackColor=Core::Color32(0,0,0,0);

	IElement::Clear();
}

int Tab::GetNumber() const
{
	return m_iNumber;
}

void Tab::SetNumber(int n)
{
	m_iNumber=n;
}

void Tab::Draw()
{
	if(!m_bIsVisible)
		return;

	ISkin *skin=m_pEnvironment->GetSkin();

	if(skin && m_bDrawBackground)
		skin->Draw2DRectangle(this,m_cBackColor,m_AbsoluteRect,&m_AbsoluteClippingRect);

	IElement::Draw();
}

void Tab::SetDrawBackground(bool bDraw)
{
	m_bDrawBackground=bDraw;
}

void Tab::SetBackgroundColor(Core::Color32 cColor)
{
	m_cBackColor=cColor;
}

bool Tab::IsDrawingBackground() const
{
	return m_bDrawBackground;
}

Core::Color32 Tab::GetBackgroundColor() const
{
	return m_cBackColor;
}

bool Tab::IsPointInside(const Core::Vector2i& p) const
{
	Core::Rect framerect(m_AbsoluteRect);

	int tabheight=m_pEnvironment->GetSkin()->GetSize(DS_BUTTONHEIGHT);
	framerect.m_vUpperLeft.y+=2;
	framerect.m_vLowerRight.y=framerect.m_vUpperLeft.y + tabheight;

	return framerect.IsPointInside(p);
}



TabControl::TabControl(IEnvironment *pEnvironment, IElement *pParent, IEventReceiver *pUser,
					   const Core::Rect &rect, bool bFillBackground,
					   bool bBorder, uint32 dwID) :
	ITabControl(pEnvironment,pParent,pUser,dwID,rect), m_iActiveTab(-1),
		m_bBorder(bBorder),m_bFillBackground(bFillBackground)
{
}

TabControl::~TabControl()
{
	while(m_Tabs.size() > 0)
		RemoveChild(m_Tabs[m_Tabs.size()-1],true);
}

void TabControl::Reset(IEnvironment *pEnvironment, IElement *pParent,IEventReceiver *pUser,
					   const Core::Rect &rect, bool bFillBackground,
					   bool bBorder, uint32 dwID)
{
	IElement::Reset(pEnvironment,pParent,pUser,dwID,rect);

	m_bBorder=bBorder;
	m_bFillBackground=bFillBackground;
}

void TabControl::Clear()
{
	while(m_Tabs.size() > 0)
		RemoveChild(m_Tabs[m_Tabs.size()-1],true);

	m_iActiveTab=-1;

	IElement::Clear();
}

ITab *TabControl::AddTab(const char *szCaption, int iID)
{
	ISkin *skin=m_pEnvironment->GetSkin();
	if(!skin)
		return 0;

	int tabheight=skin->GetSize(DS_BUTTONHEIGHT) + 2;
	Core::Rect r(1,tabheight,m_AbsoluteRect.GetWidth()-1,m_AbsoluteRect.GetHeight()-1);

	Tab *tab=new Tab(m_Tabs.size(),m_pEnvironment,this,m_pUserReceiver,r,(uint32)iID);
	tab->SetAlignment(AL_UPPERLEFT,AL_LOWERRIGHT,AL_UPPERLEFT,AL_LOWERRIGHT);
	tab->SetText(szCaption);
	tab->SetVisible(false);
	m_Tabs.push_back(tab);

	if(m_iActiveTab==-1)
	{
		m_iActiveTab=0;
		tab->SetVisible(true);
	}

	return tab;
}

void TabControl::AddTab(Tab *tab)
{
	if(!tab)
		return;

	int i;
	for(i=0;i<(int)m_Tabs.size();i++)
		if(m_Tabs[i]==tab)
			return;

	if(tab->GetNumber()==-1)
		tab->SetNumber((int)m_Tabs.size());

	while(tab->GetNumber() >= (int)m_Tabs.size())
		m_Tabs.push_back(0);

	if(m_Tabs[tab->GetNumber()])
	{
		m_Tabs.push_back(m_Tabs[tab->GetNumber()]);
		m_Tabs[m_Tabs.size()-1]->SetNumber(m_Tabs.size());
	}
	m_Tabs[tab->GetNumber()]=tab;

	if(m_iActiveTab==-1)
		m_iActiveTab=tab->GetNumber();

	if(tab->GetNumber()==m_iActiveTab)
		SetActiveTab(m_iActiveTab);
}

int TabControl::GetTabCount() const
{
	return (int)m_Tabs.size();
}

ITab *TabControl::GetTab(int iIndex) const
{
	if(iIndex < 0 || iIndex >= (int)m_Tabs.size())
		return 0;

	return m_Tabs[iIndex];
}

bool TabControl::OnEvent(const Event &_Event)
{
	if(!m_bIsEnable)
		return m_pParent ? m_pParent->OnEvent(_Event) : false;

	switch(_Event.EventType)
	{
		case ET_MOUSE:
			switch(_Event.sMouse.EventType)
			{
				case ME_LMOUSEDOWN:
					m_pEnvironment->SetFocus(this);
					return true;

				case ME_LMOUSEUP:
					m_pEnvironment->RemoveFocus(this);
					SelectTab(Core::Vector2i((int)_Event.sMouse.dwX,(int)_Event.sMouse.dwY));
					return true;

				default:
					break;
			}
			break;

		default:
			break;
	}

	return m_pParent ? m_pParent->OnEvent(_Event) : false;
}

void TabControl::SelectTab(Core::Vector2i p)
{
	ISkin *skin=m_pEnvironment->GetSkin();
	IFont *font=skin->GetFont();

	Core::Rect framerect(m_AbsoluteRect);

	int tabheight=skin->GetSize(DS_BUTTONHEIGHT);
	framerect.m_vUpperLeft.y+=2;
	framerect.m_vLowerRight.y=framerect.m_vUpperLeft.y + tabheight;
	int pos=framerect.m_vUpperLeft.x + 2;
	uint32 i;

	for(i=0;i<m_Tabs.size();i++)
	{
		const char *text=0;
		if(m_Tabs[i])
			text=m_Tabs[i]->GetText();

		int len=20;
		if(font)
			len+=font->GetDimension(text).x;

		framerect.m_vUpperLeft.x=pos;
		framerect.m_vLowerRight.x=framerect.m_vUpperLeft.x + len;
		pos+=len;

		if(framerect.IsPointInside(p))
		{
			SetActiveTab(i);
			return;
		}
	}
}

void TabControl::Draw()
{
	if(!m_bIsVisible)
		return;

	ISkin *skin=m_pEnvironment->GetSkin();
	if(!skin)
		return;

	IFont *font=skin->GetFont();

	Core::Rect framerect(m_AbsoluteRect);

	if(m_Tabs.empty())
		skin->Draw2DRectangle(this,skin->GetColor(DC_3DHIGHLIGHT),framerect,&m_AbsoluteClippingRect);

	if(!font)
		return;

	int tabheight=skin->GetSize(DS_BUTTONHEIGHT);
	framerect.m_vUpperLeft.y+=2;
	framerect.m_vLowerRight.y=framerect.m_vUpperLeft.y + tabheight;
	Core::Rect tr;
	int pos=framerect.m_vUpperLeft.x + 2;

	int left=0,right=0;
	const char *activetext=0;
	int i;
	
	for(i=0;i<(int)m_Tabs.size();i++)
	{
		const char *text=0;
		if(m_Tabs[i])
			text=m_Tabs[i]->GetText();

		int len=font->GetDimension(text).x + 20;
		framerect.m_vUpperLeft.x=pos;
		framerect.m_vLowerRight.x=framerect.m_vUpperLeft.x + len;
		pos+=len;

		if(i==m_iActiveTab)
		{
			left=framerect.m_vUpperLeft.x;
			right=framerect.m_vLowerRight.x;
			activetext=text;
		}
		else
		{
			skin->Draw3DTabButton(this,false,framerect,&m_AbsoluteClippingRect);
			font->Draw(text,framerect,skin->GetColor(DC_BUTTONTEXT),true,true,&m_AbsoluteClippingRect);
		}
	}

	if(left!=0 && right!=0)
	{
		framerect.m_vUpperLeft.x=left-2;
		framerect.m_vLowerRight.x=right+2;
		framerect.m_vUpperLeft.y-=2;

		skin->Draw3DTabButton(this,true,framerect,&m_AbsoluteClippingRect);
		font->Draw(activetext,framerect,skin->GetColor(DC_BUTTONTEXT),true,true,&m_AbsoluteClippingRect);

		tr.m_vUpperLeft.x=m_AbsoluteRect.m_vUpperLeft.x;
		tr.m_vLowerRight.x=left-1;
		tr.m_vUpperLeft.y=framerect.m_vLowerRight.y-1;
		tr.m_vLowerRight.y=framerect.m_vLowerRight.y;
		skin->Draw2DRectangle(this,skin->GetColor(DC_3DHIGHLIGHT),tr,&m_AbsoluteClippingRect);

		tr.m_vUpperLeft.x=right;
		tr.m_vLowerRight.x=m_AbsoluteRect.m_vLowerRight.x;
		skin->Draw2DRectangle(this,skin->GetColor(DC_3DHIGHLIGHT),tr,&m_AbsoluteClippingRect);
	}

	skin->Draw3DTabBody(this,m_bBorder,m_bFillBackground,m_AbsoluteRect,&m_AbsoluteClippingRect);
	IElement::Draw();
}

int TabControl::GetActiveTab() const
{
	return m_iActiveTab;
}

bool TabControl::SetActiveTab(IElement *tab)
{
	int i;
	for(i=0;i<(int)m_Tabs.size();i++)
		if(m_Tabs[i]==tab)
			return SetActiveTab(i);

	return false;
}

bool TabControl::SetActiveTab(int index)
{
	if((uint32)index >= m_Tabs.size())
		return false;

	bool changed=(m_iActiveTab!=index);
	m_iActiveTab=index;

	int i;
	for(i=0;i<(int)m_Tabs.size();i++)
		if(m_Tabs[i])
			m_Tabs[i]->SetVisible(i==m_iActiveTab);

	if(changed)
	{
		Event e;
		e.EventType=ET_GUI;
		e.sGUI.pCaller=this;
		e.sGUI.pElement=0;
		e.sGUI.EventType=GE_TABCHANGED;
		SendEvent(e);
	}

	return true;
}

void TabControl::RemoveChild(IElement *child, bool bDelete)
{
	bool istab=false;
	uint32 i=0;

	while(i< m_Tabs.size())
	{
		if(m_Tabs[i]==child)
		{
			m_Tabs.erase(m_Tabs.begin() + i);
			istab=true;
		}
		else
			i++;
	}

	if(istab)
	{
		for(i=0;i<m_Tabs.size();i++)
			if(m_Tabs[i])
				m_Tabs[i]->SetNumber(i);
	}

	IElement::RemoveChild(child,bDelete);

	if(bDelete && child)
		delete child;
}

bool TabControl::IsPointInside(const Core::Vector2i& p) const
{
	Core::Rect framerect(m_AbsoluteRect);

	int tabheight=m_pEnvironment->GetSkin()->GetSize(DS_BUTTONHEIGHT);
	framerect.m_vUpperLeft.y+=2;
	framerect.m_vLowerRight.y=framerect.m_vUpperLeft.y + tabheight;

	return framerect.IsPointInside(p);
}

}