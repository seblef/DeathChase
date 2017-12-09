
#include "ContextMenu.h"
#include "ISkin.h"
#include "IEnvironment.h"
#include "IFont.h"
#include "ISpriteBank.h"
#include <windows.h>

namespace GUI
{

ContextMenu::ContextMenu(IEnvironment *pEnvironment, IElement *pParent, IEventReceiver *pUser,
						 uint32 dwID, Core::Rect rect, bool bGetFocus, bool bAllowFocus) :
	IContextMenu(pEnvironment,pParent,pUser,dwID,rect), m_iHighLighted(-1),
		m_dwChangeTime(0),m_pEventParent(0),m_bAllowFocus(bAllowFocus)
{
	m_vPos=rect.m_vUpperLeft;
	RecalculateSize();

	if(bGetFocus)
		m_pEnvironment->SetFocus(this);

	SetNoClip(true);
}

ContextMenu::~ContextMenu()
{
	uint32 i;
	for(i=0;i<m_Items.size();i++)
		if(m_Items[i].pSubMenu)
			RemoveChild(m_Items[i].pSubMenu,true);
}

void ContextMenu::Reset(IEnvironment *pEnvironment, IElement *pParent,IEventReceiver *pUser,
						 uint32 dwID, Core::Rect rect, bool bGetFocus, bool bAllowFocus)
{
	IElement::Reset(pEnvironment,pParent,pUser,dwID,rect);

	m_bAllowFocus=bAllowFocus;

	m_vPos=rect.m_vUpperLeft;
	RecalculateSize();

	if(bGetFocus)
		m_pEnvironment->SetFocus(this);

	SetNoClip(true);
}

void ContextMenu::Clear()
{
	uint32 i;
	for(i=0;i<m_Items.size();i++)
		if(m_Items[i].pSubMenu)
			RemoveChild(m_Items[i].pSubMenu,true);

	m_Items.clear();

	m_iHighLighted=-1;
	m_dwChangeTime=0;
	m_pEventParent=0;

	IElement::Clear();
}

uint32 ContextMenu::GetItemCount() const
{
	return m_Items.size();
}

uint32 ContextMenu::AddItem(const char *szText, int iCommandID,
						 bool bEnable, bool bHasSubMenu, bool bChecked)
{
	Item s;
	s.bEnable=bEnable;
	s.bChecked=bChecked;
	s.strText=szText;
	s.bIsSeparator=(szText==0);
	s.pSubMenu=0;
	s.iCommandID=iCommandID;

	if(bHasSubMenu)
	{
		s.pSubMenu=(ContextMenu *)m_pEnvironment->AddContextMenu(Core::Rect(0,0,10,10),this,
			m_pUserReceiver,(uint32)iCommandID);
		s.pSubMenu->SetAllowFocus(true);
		m_pEnvironment->SetFocus((IElement *)s.pSubMenu);
		s.pSubMenu->SetVisible(false);
	}

	m_Items.push_back(s);

	RecalculateSize();
	return m_Items.size()-1;
}

void ContextMenu::SetSubMenu(uint32 dwIndex, ContextMenu *pMenu)
{
	if(dwIndex > m_Items.size())
		return;

	if(m_Items[dwIndex].pSubMenu)
		RemoveChild(m_Items[dwIndex].pSubMenu,true);

	m_Items[dwIndex].pSubMenu=pMenu;

	if(m_Items[dwIndex].pSubMenu)
	{
		pMenu->SetVisible(false);
		pMenu->m_bAllowFocus=false;
		if(m_pEnvironment->GetFocus()==pMenu)
			m_pEnvironment->SetFocus(this);
	}

	RecalculateSize();
}

void ContextMenu::AddSeparator()
{
	AddItem(0,-1,true,false,false);
}

const char *ContextMenu::GetItemText(uint32 dwIndex) const
{
	if(dwIndex >= m_Items.size())
		return 0;

	return m_Items[dwIndex].strText.c_str();
}

void ContextMenu::SetItemText(uint32 dwIndex, const char *szText)
{
	if(dwIndex >= m_Items.size())
		return;

	m_Items[dwIndex].strText=szText;
	RecalculateSize();
}

bool ContextMenu::IsItemEnabled(uint32 dwIndex) const
{
	if(dwIndex >= m_Items.size())
		return false;

	return m_Items[dwIndex].bEnable;
}

bool ContextMenu::IsItemChecked(uint32 dwIndex) const
{
	if(dwIndex >= m_Items.size())
		return false;

	return m_Items[dwIndex].bChecked;
}

void ContextMenu::SetItemEnabled(uint32 dwIndex, bool bEnable)
{
	if(dwIndex >= m_Items.size())
		return;

	m_Items[dwIndex].bEnable=bEnable;
}

void ContextMenu::SetItemChecked(uint32 dwIndex, bool bChecked)
{
	if(dwIndex >= m_Items.size())
		return;

	m_Items[dwIndex].bChecked=bChecked;
}

void ContextMenu::RemoveItem(uint32 dwIndex)
{
	if(dwIndex >= m_Items.size())
		return;

	if(m_Items[dwIndex].pSubMenu)
	{
		RemoveChild(m_Items[dwIndex].pSubMenu,true);
		m_Items[dwIndex].pSubMenu=0;
	}

	m_Items.erase(m_Items.begin() + dwIndex);
	RecalculateSize();
}

void ContextMenu::RemoveAllItems()
{
	uint32 i;
	for(i=0;i<m_Items.size();i++)
		if(m_Items[i].pSubMenu)
			RemoveChild(m_Items[i].pSubMenu,true);

	m_Items.clear();
	RecalculateSize();
}

bool ContextMenu::OnEvent(const GUI::Event &_Event)
{
	if(!m_bIsEnable)
		return m_pParent ? m_pParent->OnEvent(_Event) : false;

	switch(_Event.EventType)
	{
		case ET_GUI:
			switch(_Event.sGUI.EventType)
			{
				case GE_FOCUSLOST:
					if(_Event.sGUI.pCaller==this && !IsMyChild(_Event.sGUI.pElement) && m_bAllowFocus)
					{
						SetEventParent(m_pParent);
						SetVisible(false);
						return false;
					}
					break;

				case GE_FOCUSED:
					if(_Event.sGUI.pCaller==this && !m_bAllowFocus)
						return true;

					break;

				default:
					break;
			}

		case ET_MOUSE:
			switch(_Event.sMouse.EventType)
			{
				case ME_LMOUSEUP:
					{
						const uint32 t=SendClick(Core::Vector2i((int)_Event.sMouse.dwX,(int)_Event.sMouse.dwY));
						if((t==0 || t==1) && m_pEnvironment->HasFocus(this))
							m_pEnvironment->RemoveFocus(this);
					}
					return true;

				case ME_LMOUSEDOWN:
					return true;

				case ME_MOVED:
					if(m_pEnvironment->HasFocus(this))
						Highlight(Core::Vector2i((int)_Event.sMouse.dwX,(int)_Event.sMouse.dwY),true);
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

void ContextMenu::SetVisible(const bool bVisible)
{
	m_iHighLighted=-1;
	m_dwChangeTime=timeGetTime();
	
	uint32 j;
	for(j=0;j<m_Items.size();j++)
		if(m_Items[j].pSubMenu)
			m_Items[j].pSubMenu->SetVisible(false);

	IElement::SetVisible(bVisible);
}

uint32 ContextMenu::SendClick(const Core::Vector2i& p)
{
	uint32 t=0;

	int openmenu=-1;
	int j;

	for(j=0;j<(int)m_Items.size();j++)
		if(m_Items[j].pSubMenu && m_Items[j].pSubMenu->IsVisible())
		{
			openmenu=j;
			break;
		}

	if(openmenu!=-1)
	{
		t=m_Items[j].pSubMenu->SendClick(p);
		if(t!=0)
			return t;
	}

	if(IsPointInside(p) && (uint32)m_iHighLighted < m_Items.size())
	{
		if(!m_Items[m_iHighLighted].bEnable ||
			m_Items[m_iHighLighted].bIsSeparator ||
			m_Items[m_iHighLighted].pSubMenu)
			return 2;

		Event e;
		e.EventType=ET_GUI;
		e.sGUI.pCaller=this;
		e.sGUI.pElement=0;
		e.sGUI.EventType=GE_MENUSELECTED;

		if(m_pParent)
			m_pParent->OnEvent(e);
		else if(m_pEventParent)
			m_pEventParent->OnEvent(e);

		if(m_pUserReceiver)
			m_pUserReceiver->OnEvent(e);

		return 1;
	}

	return 0;
}

bool ContextMenu::Highlight(const Core::Vector2i& p, bool bCanOpenSubMenu)
{
	int openmenu=-1;
	int i;

	for(i=0;i<(int)m_Items.size();i++)
		if(m_Items[i].pSubMenu && m_Items[i].pSubMenu->IsVisible())
		{
			openmenu=i;
			break;
		}

	if(openmenu!=-1)
	{
		if(m_Items[openmenu].pSubMenu->Highlight(p,bCanOpenSubMenu))
		{
			m_iHighLighted=openmenu;
			m_dwChangeTime=timeGetTime();
			return true;
		}
	}

	for(i=0;i<(int)m_Items.size();i++)
		if(GetHRect(m_Items[i],m_AbsoluteRect).IsPointInside(p))
		{
			m_iHighLighted=i;
			m_dwChangeTime=timeGetTime();

			int j;
			for(j=0;j<m_Items.size();j++)
				if(m_Items[j].pSubMenu)
				{
					if(j==i && bCanOpenSubMenu)
						m_Items[j].pSubMenu->SetVisible(true);
					else
						m_Items[j].pSubMenu->SetVisible(false);
				}

			return true;
		}

	m_iHighLighted=openmenu;
	return false;
}

Core::Rect ContextMenu::GetHRect(const Item &i, const Core::Rect &absolute) const
{
	Core::Rect r(absolute);
	r.m_vUpperLeft.y+=i.iPosY;
	r.m_vLowerRight.y=r.m_vUpperLeft.y + i.vDim.y;
	return r;
}

Core::Rect ContextMenu::GetRect(const Item& i, const Core::Rect& absolute) const
{
	Core::Rect r(absolute);
	r.m_vUpperLeft.y+=i.iPosY;
	r.m_vLowerRight.y=r.m_vUpperLeft.y + i.vDim.y;
	r.m_vUpperLeft.x+=20;
	return r;
}

void ContextMenu::Draw()
{
	if(!m_bIsVisible)
		return;

	ISkin *skin=m_pEnvironment->GetSkin();
	if(!skin)
		return;

	IFont *font=skin->GetFont(DF_MENU);
	ISpriteBank *sprites=skin->GetSpriteBank();

	Core::Rect rect(m_AbsoluteRect);
	Core::Rect *clip=0;

	skin->Draw3DMenuPane(this,m_AbsoluteRect,clip);

	rect=m_AbsoluteRect;
	int y=m_AbsoluteRect.m_vUpperLeft.y;
	int i;

	for(i=0;i<(int)m_Items.size();i++)
	{
		if(m_Items[i].bIsSeparator)
		{
			rect=m_AbsoluteRect;
			rect.m_vUpperLeft.y+=m_Items[i].iPosY + 3;
			rect.m_vLowerRight.y=rect.m_vUpperLeft.y + 1;
			rect.m_vUpperLeft.x+=5;
			rect.m_vLowerRight.x-=5;
			skin->Draw2DRectangle(this,skin->GetColor(DC_3DSHADOW),rect,clip);

			rect.m_vLowerRight.y+=1;
			rect.m_vUpperLeft.y+=1;
			skin->Draw2DRectangle(this,skin->GetColor(DC_3DHIGHLIGHT),rect,clip);

			y+=10;
		}
		else
		{
			rect=GetRect(m_Items[i],m_AbsoluteRect);

			if(i==m_iHighLighted && m_Items[i].bEnable)
			{
				Core::Rect r(m_AbsoluteRect);
				r.m_vLowerRight.y=rect.m_vLowerRight.y;
				r.m_vUpperLeft.y=rect.m_vUpperLeft.y;
				r.m_vLowerRight.x-=5;
				r.m_vUpperLeft.x+=5;
				skin->Draw2DRectangle(this,skin->GetColor(DC_HIGHLIGHT),r,clip);
			}

			DEFAULTCOLOR c=DC_BUTTONTEXT;

			if(i==m_iHighLighted) c=DC_HIGHLIGHTTEXT;
			if(!m_Items[i].bEnable) c=DC_GRAYTEXT;

			if(font)
				font->Draw(m_Items[i].strText.c_str(),rect,
					skin->GetColor(c),false,true,clip);

			if(m_Items[i].pSubMenu && sprites)
			{
				Core::Rect r(rect);
				r.m_vUpperLeft.x=r.m_vLowerRight.x - 15;

				sprites->DrawSprite(skin->GetIcon(DI_CURSORRIGHT),
					r.GetCenter(),clip,skin->GetColor(c),
					(i==m_iHighLighted) ? m_dwChangeTime : 0,
					(i==m_iHighLighted) ? timeGetTime() : 0,
					(i==m_iHighLighted), true);
			}

			if(m_Items[i].bChecked && sprites)
			{
				Core::Rect r(rect);
				r.m_vLowerRight.x=r.m_vUpperLeft.x - 15;
				r.m_vUpperLeft.x=r.m_vLowerRight.x + 15;
				sprites->DrawSprite(skin->GetIcon(DI_CHECKBOXCHECKED),
					r.GetCenter(),clip,skin->GetColor(c),
					(i==m_iHighLighted) ? m_dwChangeTime : 0,
					(i==m_iHighLighted) ? timeGetTime() : 0,
					(i==m_iHighLighted), true);
			}
		}
	}

	IElement::Draw();
}

void ContextMenu::RecalculateSize()
{
	ISkin *skin=m_pEnvironment->GetSkin();
	IFont *font=skin->GetFont(DF_MENU);

	if(!font)
		return;

	Core::Rect rect;
	rect.m_vUpperLeft=m_RelativeRect.m_vUpperLeft;
	int width=100;
	int height=3;
	int i;

	for(i=0;i<(int)m_Items.size();i++)
	{
		if(m_Items[i].bIsSeparator)
		{
			m_Items[i].vDim.x=100;
			m_Items[i].vDim.y=10;
		}
		else
		{
			m_Items[i].vDim=font->GetDimension(m_Items[i].strText.c_str());
			m_Items[i].vDim.x+=40;

			if(m_Items[i].vDim.x > width)
				width=m_Items[i].vDim.x;
		}

		m_Items[i].iPosY=height;
		height+=m_Items[i].vDim.y;
	}

	height+=5;
	if(height < 10) height=10;

	rect.m_vLowerRight.x=m_RelativeRect.m_vUpperLeft.x + width;
	rect.m_vLowerRight.y=m_RelativeRect.m_vUpperLeft.y + height;

	SetRelativePosition(rect);

	for(i=0;i<(int)m_Items.size();i++)
	{
		if(m_Items[i].pSubMenu)
		{
			const int w=m_Items[i].pSubMenu->GetAbsolutePosition().GetWidth();
			const int h=m_Items[i].pSubMenu->GetAbsolutePosition().GetHeight();

			m_Items[i].pSubMenu->SetRelativePosition(Core::Rect(width-5,m_Items[i].iPosY,
				width+w-5,m_Items[i].iPosY + h));
		}
	}
}

int ContextMenu::GetSelectedItem() const
{
	return m_iHighLighted;
}

IContextMenu *ContextMenu::GetSubMenu(uint32 dwIndex) const
{
	if(dwIndex >= m_Items.size())
		return 0;

	return m_Items[dwIndex].pSubMenu;
}

int ContextMenu::GetItemCommandID(uint32 dwIndex) const
{
	if(dwIndex >= m_Items.size())
		return -1;

	return m_Items[dwIndex].iCommandID;
}

void ContextMenu::SetItemCommandID(uint32 dwIndex, int iID)
{
	if(dwIndex >= m_Items.size())
		return;

	m_Items[dwIndex].iCommandID=iID;
}

void ContextMenu::SetEventParent(IElement *pParent)
{
	m_pEventParent=pParent;
	uint32 i;

	for(i=0;i<m_Items.size();i++)
		if(m_Items[i].pSubMenu)
			m_Items[i].pSubMenu->SetEventParent(pParent);
}

bool ContextMenu::HasOpenSubMenu() const
{
	uint32 i;
	for(i=0;i<m_Items.size();i++)
		if(m_Items[i].pSubMenu && m_Items[i].pSubMenu->IsVisible())
			return true;

	return false;
}

void ContextMenu::CloseAllSubMenus()
{
	uint32 i;
	for(i=0;i<m_Items.size();i++)
		if(m_Items[i].pSubMenu)
			m_Items[i].pSubMenu->SetVisible(false);
}

}
