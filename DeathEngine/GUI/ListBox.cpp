
#include "ListBox.h"
#include "ISkin.h"
#include "IEnvironment.h"
#include "IFont.h"
#include "ISpriteBank.h"
#include "ScrollBar.h"
#include <dinput.h>

namespace GUI
{

ListBox::ListBox(IEnvironment *pEnvironment, IElement *pParent, IEventReceiver *pUser, uint32 dwID,
				 Core::Rect rect, bool bClip, bool bDrawBack, bool bMoveOverSelect) :
	IListBox(pEnvironment,pParent,pUser,dwID,rect), m_iSelected(-1), m_iItemHeight(0),
		m_iTotalItemHeight(0),m_iItemsIconWidth(0),m_pFont(0),m_pIconBank(0),
		m_pScrollBar(0),m_bSelecting(false),m_bDrawBack(true),//bDrawBack),
		m_bMoveOverSelect(bMoveOverSelect), m_dwSelectTime(0), m_bAutoScroll(true),
		m_dwLastKeyTime(0), m_bHighLightWhenNotFocused(true)
{
	ISkin *skin=m_pEnvironment->GetSkin();
	const int s=skin->GetSize(DS_SCROLLBARSIZE);

	m_pScrollBar=m_pEnvironment->AddScrollBar(false,
		Core::Rect(m_RelativeRect.GetWidth()-s,0,m_RelativeRect.GetWidth(),m_RelativeRect.GetHeight()),
		this,0,INVALID_ID);
	m_pScrollBar->SetNoClip(!bClip);
	m_pScrollBar->SetSubElement(true);
	m_pScrollBar->SetTabStop(false);
	m_pScrollBar->SetAlignment(AL_LOWERRIGHT,AL_LOWERRIGHT,AL_UPPERLEFT,AL_LOWERRIGHT);
	m_pScrollBar->SetVisible(false);
	m_pScrollBar->SetPos(0);

	SetNoClip(!bClip);

	SetTabStop(true);
	SetTabOrder(-1);

	UpdateAbsolutePosition();
}

ListBox::~ListBox()
{
	RemoveChild(m_pScrollBar,true);
	if(m_pFont) m_pFont->Drop();
	if(m_pIconBank) m_pIconBank->Drop();
}

void ListBox::Reset(IEnvironment *pEnvironment, IElement *pParent, IEventReceiver *pUser, uint32 dwID,
				 Core::Rect rect, bool bClip, bool bDrawBack, bool bMoveOverSelect)
{
	IElement::Reset(pEnvironment,pParent,pUser,dwID,rect);

	m_bDrawBack=bDrawBack;
	m_bMoveOverSelect=bMoveOverSelect;
	ISkin *skin=m_pEnvironment->GetSkin();
	const int s=skin->GetSize(DS_SCROLLBARSIZE);

	m_pScrollBar=m_pEnvironment->AddScrollBar(false,
		Core::Rect(m_RelativeRect.GetWidth()-s,0,m_RelativeRect.GetWidth(),m_RelativeRect.GetHeight()),
		this,0,INVALID_ID);
	m_pScrollBar->SetNoClip(!bClip);
	m_pScrollBar->SetSubElement(true);
	m_pScrollBar->SetTabStop(false);
	m_pScrollBar->SetAlignment(AL_LOWERRIGHT,AL_LOWERRIGHT,AL_UPPERLEFT,AL_LOWERRIGHT);
	m_pScrollBar->SetVisible(false);
	m_pScrollBar->SetPos(0);

	SetNoClip(!bClip);

	SetTabStop(true);
	SetTabOrder(-1);

	UpdateAbsolutePosition();
}

void ListBox::Clear()
{
	RemoveChild(m_pScrollBar,true);
	if(m_pFont) m_pFont->Drop();
	if(m_pIconBank) m_pIconBank->Drop();

	m_iSelected=-1;
	m_iItemHeight=0;
	m_iTotalItemHeight=0;
	m_iItemsIconWidth=0;
	m_pFont=0;
	m_pIconBank=0;
	m_pScrollBar=0;
	m_bSelecting=false;

	m_dwSelectTime=0;
	m_bAutoScroll=true;
	m_dwLastKeyTime=0;
	m_bHighLightWhenNotFocused=true;
	m_Items.clear();

	IElement::Clear();
}

uint32 ListBox::GetItemCount() const
{
	return m_Items.size();
}

const char *ListBox::GetListItem(uint32 dwIndex) const
{
	if(dwIndex >= m_Items.size())
		return 0;

	return m_Items[dwIndex].strText.c_str();
}

int ListBox::GetIcon(uint32 dwIndex) const
{
	if(dwIndex >= m_Items.size())
		return -1;

	return m_Items[dwIndex].iIcon;
}

uint32 ListBox::AddItem(const char *szText)
{
	return AddItem(szText,-1);
}

void ListBox::RemoveItem(uint32 dwIndex)
{
	if(dwIndex >= m_Items.size())
		return;

	if((uint32)m_iSelected==dwIndex)
		m_iSelected=-1;
	else if((uint32)m_iSelected > dwIndex)
	{
		m_iSelected=-1;
		m_dwSelectTime=timeGetTime();
	}

	m_Items.erase(m_Items.begin() + dwIndex);
	RecalculateItemHeight();
}

void ListBox::ClearList()
{
	m_Items.clear();
	m_iItemsIconWidth=0;
	m_iSelected=-1;

	if(m_pScrollBar)
		m_pScrollBar->SetPos(0);

	RecalculateItemHeight();
}

void ListBox::RecalculateItemHeight()
{
	ISkin *skin=m_pEnvironment->GetSkin();

	if(m_pFont!=skin->GetFont())
	{
		if(m_pFont) m_pFont->Drop();

		m_pFont=skin->GetFont();
		m_iItemHeight=0;

		if(m_pFont)
		{
			m_iItemHeight=m_pFont->GetDimension("A").y + 4;
			m_pFont->Grab();
		}
	}

	m_iTotalItemHeight=m_iItemHeight * m_Items.size();

	if(m_pScrollBar)
	{
		m_pScrollBar->SetMax(m_iTotalItemHeight - m_AbsoluteRect.GetHeight());

		if(m_iTotalItemHeight <= m_AbsoluteRect.GetHeight())
			m_pScrollBar->SetVisible(false);
		else
			m_pScrollBar->SetVisible(true);
	}
}

int ListBox::GetSelected() const
{
	return m_iSelected;
}

void ListBox::SetSelected(int iIndex)
{
	if(iIndex >= m_Items.size())
		m_iSelected=-1;
	else
		m_iSelected=iIndex;

	m_dwSelectTime=timeGetTime();
	RecalculateScrollPos();
}

bool ListBox::OnEvent(const Event& _Event)
{
	switch(_Event.EventType)
	{
		case ET_KEY:
			if(_Event.sKey.bPressed &&
				(_Event.sKey.cDXChar==DIK_DOWN ||
				_Event.sKey.cDXChar==DIK_UP ||
				_Event.sKey.cDXChar==DIK_HOME ||
				_Event.sKey.cDXChar==DIK_END ||
				_Event.sKey.cDXChar==DIK_NEXT ||
				_Event.sKey.cDXChar==DIK_PRIOR))
			{
				int oldselected=m_iSelected;
				switch(_Event.sKey.cDXChar)
				{
					case DIK_DOWN:
						m_iSelected+=1;
						break;
					case DIK_UP:
						m_iSelected-=1;
						break;
					case DIK_HOME:
						m_iSelected=0;
						break;
					case DIK_END:
						m_iSelected=(int)m_Items.size() -1;
						break;
					case DIK_NEXT:
						m_iSelected+=m_AbsoluteRect.GetHeight() / m_iItemHeight;
						break;
					case DIK_PRIOR:
						m_iSelected-=m_AbsoluteRect.GetHeight() / m_iItemHeight;
						break;
					default:
						break;
				}

				if(m_iSelected >= (int)m_Items.size()) m_iSelected=(int)m_Items.size() -1;
				else if(m_iSelected < 0) m_iSelected=0;

				RecalculateScrollPos();

				if(oldselected!=m_iSelected && m_pParent && !m_bSelecting && !m_bMoveOverSelect)
				{
					Event e;
					e.EventType=ET_GUI;
					e.sGUI.pCaller=this;
					e.sGUI.pElement=0;
					e.sGUI.EventType=GE_LISTBOXCHANGED;
					SendEvent(e);
				}
				return true;
			}
			else if(_Event.sKey.bPressed && (_Event.sKey.cDXChar==DIK_RETURN || _Event.sKey.cDXChar==DIK_SPACE))
			{
				if(m_pParent)
				{
					Event e;
					e.EventType=ET_GUI;
					e.sGUI.pCaller=this;
					e.sGUI.pElement=0;
					e.sGUI.EventType=GE_LISTBOXSELECTEDAGAIN;
					SendEvent(e);
				}
				return true;
			}
			else if(_Event.sKey.bPressed && _Event.sKey.cChar)
			{
				uint32 now=timeGetTime();
				if(now - m_dwLastKeyTime < 500)
				{
					if(!(m_strKeyBuffer.size()==1 && m_strKeyBuffer[0]==_Event.sKey.cChar))
					{
						m_strKeyBuffer+=" ";
						m_strKeyBuffer[m_strKeyBuffer.size()-1]=_Event.sKey.cChar;
					}
				}
				else
				{
					m_strKeyBuffer=" ";
					m_strKeyBuffer[0]=_Event.sKey.cChar;
				}
				m_dwLastKeyTime=now;

				int start=m_iSelected;
				if(m_iSelected > -1 && m_strKeyBuffer.size() > 1)
				{
					// TODO: compare without case

					if(m_Items[m_iSelected].strText.size() >= m_strKeyBuffer.size() &&
						m_strKeyBuffer.compare(m_Items[m_iSelected].strText.substr(0,m_strKeyBuffer.size()))==0)
						return true;
				}

				int current;
				for(current=start+1;current<(int)m_Items.size();current++)
				{
					if(m_Items[current].strText.size() >= m_strKeyBuffer.size())
					{
						// TODO: compare without case
						if(m_strKeyBuffer.compare(m_Items[current].strText.substr(0,m_strKeyBuffer.size()))==0)
						{
							if(m_pParent && m_iSelected!=current && !m_bSelecting && !m_bMoveOverSelect)
							{
								Event e;
								e.EventType=ET_GUI;
								e.sGUI.pCaller=this;
								e.sGUI.pElement=0;
								e.sGUI.EventType=GE_LISTBOXCHANGED;
								SendEvent(e);
							}
							SetSelected(current);
							return true;
						}
					}
				}

				for(current=0;current<=start;current++)
				{
					if(m_Items[current].strText.size() >= m_strKeyBuffer.size())
					{
						// TODO: compare without case
						if(m_strKeyBuffer.compare(m_Items[current].strText.substr(0,m_strKeyBuffer.size()))==0)
						{
							if(m_pParent && m_iSelected!=current && !m_bSelecting && !m_bMoveOverSelect)
							{
								m_iSelected=current;
								Event e;
								e.EventType=ET_GUI;
								e.sGUI.pCaller=this;
								e.sGUI.pElement=0;
								e.sGUI.EventType=GE_LISTBOXCHANGED;
								SendEvent(e);
							}
							SetSelected(current);
							return true;
						}
					}
				}
				return true;
			}
			break;

		case ET_GUI:
			switch(_Event.sGUI.EventType)
			{
				case GE_SCROLLBARCHANGED:
					if(_Event.sGUI.pCaller==m_pScrollBar)
						return true;
					break;
				case GE_FOCUSLOST:
					{
						if(_Event.sGUI.pCaller==this)
							m_bSelecting=false;
					}
					break;
				default:
					break;
			}
			break;

		case ET_MOUSE:
			{
				Core::Vector2i p((int)_Event.sMouse.dwX,(int)_Event.sMouse.dwY);

				switch(_Event.sMouse.EventType)
				{
					case ME_MOUSEWHEEL:
						m_pScrollBar->SetPos(m_pScrollBar->GetPos() + (int)(_Event.sMouse.fWheel*-10.0f));
						return true;
						break;

					case ME_LMOUSEDOWN:
						m_bSelecting=true;
						return true;
						break;

					case ME_LMOUSEUP:
						m_bSelecting=false;

						if(IsPointInside(p))
							SelectNew(p.y);

						return true;
						break;

					case ME_MOVED:
						if(m_bSelecting || m_bMoveOverSelect)
						{
							if(IsPointInside(p))
							{
								SelectNew((int)_Event.sMouse.dwY,true);
								return true;
							}
						}
						break;
						
					default:
						break;
				}
			}
			break;
	}

	return m_pParent ? m_pParent->OnEvent(_Event) : false;
}

void ListBox::SelectNew(int ypos, bool bOnlyHover)
{
	int oldselected=m_iSelected;

	if(m_iItemHeight!=0)
		m_iSelected=((ypos - m_AbsoluteRect.m_vUpperLeft.y - 1) + m_pScrollBar->GetPos()) / m_iItemHeight;

	if(m_iSelected < 0) m_iSelected=0;
	else if(m_iSelected >= (int)m_Items.size())
		m_iSelected=m_Items.size()-1;

	RecalculateScrollPos();

	if(m_pParent && !bOnlyHover)
	{
		uint32 now=timeGetTime();

		Event e;
		e.EventType=ET_GUI;
		e.sGUI.pCaller=this;
		e.sGUI.pElement=0;
		e.sGUI.EventType=(m_iSelected==oldselected && now < m_dwSelectTime + 500) ? GE_LISTBOXSELECTEDAGAIN : GE_LISTBOXCHANGED;
		SendEvent(e);

		m_dwSelectTime=now;
	}
}

void ListBox::UpdateAbsolutePosition()
{
	IElement::UpdateAbsolutePosition();
	RecalculateItemHeight();
}

void ListBox::Draw()
{
	if(!m_bIsVisible)
		return;

	RecalculateItemHeight();

	ISkin *skin=m_pEnvironment->GetSkin();

	Core::Rect *cliprect=0;
	Core::Rect framerect(m_AbsoluteRect);
	Core::Rect clientclip(m_AbsoluteRect);

	clientclip.m_vUpperLeft.x+=1;
	clientclip.m_vUpperLeft.y+=1;

	if(m_pScrollBar->IsVisible())
		clientclip.m_vLowerRight.x=m_AbsoluteRect.m_vLowerRight.x - skin->GetSize(DS_SCROLLBARSIZE);

	clientclip.m_vLowerRight.y-=1;
	clientclip.ClipAgainst(m_AbsoluteClippingRect);

	skin->Draw3DSunkenPane(this,skin->GetColor(DC_3DHIGHLIGHT),true,m_bDrawBack,framerect,&clientclip);

	if(cliprect)
		clientclip.ClipAgainst(*cliprect);

	framerect=m_AbsoluteRect;
	framerect.m_vUpperLeft.x+=1;
	if(m_pScrollBar->IsVisible())
		framerect.m_vLowerRight.x=m_AbsoluteRect.m_vLowerRight.x - skin->GetSize(DS_SCROLLBARSIZE);

	framerect.m_vLowerRight.y=m_AbsoluteRect.m_vUpperLeft.y + m_iItemHeight;
	framerect.m_vUpperLeft.y-=m_pScrollBar->GetPos();
	framerect.m_vLowerRight.y-=m_pScrollBar->GetPos();

	bool hl=(m_bHighLightWhenNotFocused || m_pEnvironment->HasFocus(this) || m_pEnvironment->HasFocus(m_pScrollBar));
	int i;

	Core::Rect framerect2(framerect);

	for(i=0;i<(int)m_Items.size();i++)
	{
		if(framerect.m_vLowerRight.y >= m_AbsoluteRect.m_vUpperLeft.y &&
			framerect.m_vUpperLeft.y <= m_AbsoluteRect.m_vLowerRight.y)
		{
			if(i==m_iSelected && hl)
				skin->Draw2DRectangle(this,skin->GetColor(DC_HIGHLIGHT),framerect,&clientclip);
		}

		framerect.m_vUpperLeft.y+=m_iItemHeight;
		framerect.m_vLowerRight.y+=m_iItemHeight;
	}

	framerect=framerect2;

	for(i=0;i<(int)m_Items.size();i++)
	{
		if(framerect.m_vLowerRight.y >= m_AbsoluteRect.m_vUpperLeft.y &&
			framerect.m_vUpperLeft.y <= m_AbsoluteRect.m_vLowerRight.y)
		{
			Core::Rect textrect(framerect);
			textrect.m_vUpperLeft.x+=3;

			if(m_pFont)
			{
				if(m_pIconBank && (m_Items[i].iIcon > -1))
				{
					Core::Vector2i iconpos(textrect.m_vUpperLeft);
					iconpos.y+=textrect.GetHeight() / 2;
					iconpos.x+=m_iItemsIconWidth / 2;

					if(i==m_iSelected && hl)
					{
						m_pIconBank->DrawSprite((uint32)m_Items[i].iIcon,iconpos,&clientclip,
							HasItemOverrideColor(i,LBC_ICONHIGHLIGHT) ? GetItemOverrideColor(i,LBC_ICONHIGHLIGHT) : GetItemDefaultColor(LBC_ICONHIGHLIGHT),
							m_dwSelectTime,timeGetTime(),false,true);
					}
					else
					{
						m_pIconBank->DrawSprite((uint32)m_Items[i].iIcon,iconpos,&clientclip,
							HasItemOverrideColor(i,LBC_ICON) ? GetItemOverrideColor(i,LBC_ICON) : GetItemDefaultColor(LBC_ICON),
							0,(i==m_iSelected) ? timeGetTime() : 0,false,true);
					}
				}

				textrect.m_vUpperLeft.x+=m_iItemsIconWidth+3;

				if(i==m_iSelected && hl)
				{
					m_pFont->Draw(m_Items[i].strText.c_str(),textrect,
						HasItemOverrideColor(i,LBC_TEXTHIGHLIGHT) ? GetItemOverrideColor(i,LBC_TEXTHIGHLIGHT) : GetItemDefaultColor(LBC_TEXTHIGHLIGHT),
						false,true,&clientclip);
				}
				else
				{
					m_pFont->Draw(m_Items[i].strText.c_str(),textrect,
						HasItemOverrideColor(i,LBC_TEXT) ? GetItemOverrideColor(i,LBC_TEXT) : GetItemDefaultColor(LBC_TEXT),
						false,true,&clientclip);
				}

				textrect.m_vUpperLeft.x-=m_iItemsIconWidth+3;
			}
		}

		framerect.m_vUpperLeft.y+=m_iItemHeight;
		framerect.m_vLowerRight.y+=m_iItemHeight;
	}

	IElement::Draw();
}

uint32 ListBox::AddItem(const char *szText, int iIcon)
{
	ListItem i;
	i.strText=szText;
	i.iIcon=iIcon;

	m_Items.push_back(i);
	RecalculateItemHeight();
	RecalculateItemWidth(iIcon);

	return m_Items.size() - 1;
}

void ListBox::SetSpriteBank(ISpriteBank *pBank)
{
	if(m_pIconBank) m_pIconBank->Drop();

	m_pIconBank=pBank;
	if(m_pIconBank) m_pIconBank->Grab();
}

void ListBox::RecalculateScrollPos()
{
	if(!m_bAutoScroll)
		return;

	const int selpos=(m_iSelected==-1 ? m_iTotalItemHeight : m_iSelected * m_iItemHeight) - m_pScrollBar->GetPos();

	if(selpos < 0)
		m_pScrollBar->SetPos(m_pScrollBar->GetPos() + selpos);
	else if(selpos > m_AbsoluteRect.GetHeight() - m_iItemHeight)
		m_pScrollBar->SetPos(m_pScrollBar->GetPos() + selpos - m_AbsoluteRect.GetHeight() + m_iItemHeight);
}

void ListBox::SetAutoScrollEnable(bool bScroll)
{
	m_bAutoScroll=bScroll;
}

bool ListBox::IsAutoScrollEnable() const
{
	return m_bAutoScroll;
}

void ListBox::RecalculateItemWidth(int icon)
{
	if(m_pIconBank && icon > -1 &&
		m_pIconBank->GetSprites().size() > (uint32)icon &&
		m_pIconBank->GetSprites()[icon].Frames.size())
	{
		uint32 rno=m_pIconBank->GetSprites()[icon].Frames[0].wRectNum;
		if(m_pIconBank->GetPositions().size() > rno)
		{
			const int w=m_pIconBank->GetPositions()[rno].GetWidth();
			if(w > m_iItemsIconWidth)
				m_iItemsIconWidth=w;
		}
	}
}

void ListBox::SetItem(uint32 index, const char *szText, int icon)
{
	if(index >= m_Items.size())
		return;

	m_Items[index].strText=szText;
	m_Items[index].iIcon=icon;

	RecalculateItemHeight();
	RecalculateItemWidth(icon);
}

int ListBox::InsertItem(uint32 index, const char *szText, int icon)
{
	ListItem i;
	i.strText=szText;
	i.iIcon=icon;

	m_Items.insert(m_Items.begin() + index,i);
	RecalculateItemHeight();
	RecalculateItemWidth(icon);

	return (int)index;
}

void ListBox::SwapItems(uint32 dwIndex1, uint32 dwIndex2)
{
	if(dwIndex1 >= m_Items.size() || dwIndex2 >= m_Items.size())
		return;

	ListItem temp=m_Items[dwIndex1];
	m_Items[dwIndex1]=m_Items[dwIndex2];
	m_Items[dwIndex2]=temp;
}

void ListBox::SetItemOverrideColor(uint32 index, const Core::Color32& color)
{
	uint32 c;
	for(c=0;c<LBC_COUNT;c++)
	{
		m_Items[index].OverrideColors[c].bUse=true;
		m_Items[index].OverrideColors[c].cColor=color;
	}
}

void ListBox::SetItemOverrideColor(uint32 index, LISTBOX_COLOR type, const Core::Color32& color)
{
	if(index >= m_Items.size() || type < 0 || type >= LBC_COUNT)
		return;

	m_Items[index].OverrideColors[type].bUse=true;
	m_Items[index].OverrideColors[type].cColor=color;
}

void ListBox::ClearItemOverrideColor(uint32 index)
{
	if(index >= m_Items.size())
		return;

	uint32 c;
	for(c=0;c<(uint32)LBC_COUNT;c++)
		m_Items[index].OverrideColors[c].bUse=false;
}

void ListBox::ClearItemOverrideColor(uint32 index,LISTBOX_COLOR type)
{
	if(index > m_Items.size() || type < 0 || type >= LBC_COUNT)
		return;

	m_Items[index].OverrideColors[type].bUse=false;
}

bool ListBox::HasItemOverrideColor(uint32 index, LISTBOX_COLOR type) const
{
	if(index >= m_Items.size() || type < 0 || type >= LBC_COUNT)
		return false;

	return m_Items[index].OverrideColors[type].bUse;
}

Core::Color32 ListBox::GetItemOverrideColor(uint32 index, LISTBOX_COLOR type) const
{
	if(index >= m_Items.size() || type < 0 || type >= LBC_COUNT)
		return Core::Color32();

	return m_Items[index].OverrideColors[type].cColor;
}

Core::Color32 ListBox::GetItemDefaultColor(LISTBOX_COLOR type) const
{
	ISkin *skin=m_pEnvironment->GetSkin();
	if(!skin)
		return Core::Color32();

	switch(type)
	{
		case LBC_TEXT:
			return skin->GetColor(DC_BUTTONTEXT);
		case LBC_TEXTHIGHLIGHT:
			return skin->GetColor(DC_HIGHLIGHTTEXT);
		case LBC_ICON:
			return skin->GetColor((DEFAULTCOLOR)DC_ICON);
		case LBC_ICONHIGHLIGHT:
			return skin->GetColor(DC_ICONHIGHLIGHT);
		default:
			return Core::Color32();
	}
}

}