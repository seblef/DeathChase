
#ifdef DEDEV

#include "ConsoleList.h"
#include "ISkin.h"
#include "IFont.h"
#include "IEnvironment.h"
#include "ISpriteBank.h"
#include "IScrollBar.h"
#include <windows.h>

namespace GUI
{

ConsoleList::ConsoleList(int iLineCount,IEnvironment *pEnvironment, IElement *pParent,
						 IEventReceiver *pUser, uint32 dwID, Core::Rect rect,
						 bool bClip, bool bDrawBack, bool bMoveOverSelect) :
	ListBox(pEnvironment,pParent,pUser,dwID,rect,bClip,bDrawBack,bMoveOverSelect),
		m_iLineCount(iLineCount),m_iOutPos(0)
{
}

ConsoleList::~ConsoleList()
{
}

void ConsoleList::Draw()
{
	if(GetItemCount() < m_iLineCount)
	{
		ListBox::Draw();
		return;
	}

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

	for(i=0;i<(int)m_Items.size();i++)
	{
		int iDrawPos=i+m_iOutPos+1;
		if(iDrawPos >= m_Items.size())
			iDrawPos-=m_Items.size();

		if(framerect.m_vLowerRight.y >= m_AbsoluteRect.m_vUpperLeft.y &&
			framerect.m_vUpperLeft.y <= m_AbsoluteRect.m_vLowerRight.y)
		{
			if(i==m_iSelected && hl)
				skin->Draw2DRectangle(this,skin->GetColor(DC_HIGHLIGHT),framerect,&clientclip);

			Core::Rect textrect(framerect);
			textrect.m_vUpperLeft.x+=3;

			if(m_pFont)
			{
				if(m_pIconBank && (m_Items[iDrawPos].iIcon > -1))
				{
					Core::Vector2i iconpos(textrect.m_vUpperLeft);
					iconpos.y+=textrect.GetHeight() / 2;
					iconpos.x+=m_iItemsIconWidth / 2;

					if(i==m_iSelected && hl)
					{
						m_pIconBank->DrawSprite((uint32)m_Items[iDrawPos].iIcon,iconpos,&clientclip,
							HasItemOverrideColor(iDrawPos,LBC_ICONHIGHLIGHT) ? GetItemOverrideColor(iDrawPos,LBC_ICONHIGHLIGHT) : GetItemDefaultColor(LBC_ICONHIGHLIGHT),
							m_dwSelectTime,timeGetTime(),false,true);
					}
					else
					{
						m_pIconBank->DrawSprite((uint32)m_Items[iDrawPos].iIcon,iconpos,&clientclip,
							HasItemOverrideColor(iDrawPos,LBC_ICON) ? GetItemOverrideColor(iDrawPos,LBC_ICON) : GetItemDefaultColor(LBC_ICON),
							0,(iDrawPos==m_iSelected) ? timeGetTime() : 0,false,true);
					}
				}

				textrect.m_vUpperLeft.x+=m_iItemsIconWidth+3;

				if(i==m_iSelected && hl)
				{
					m_pFont->Draw(m_Items[iDrawPos].strText.c_str(),textrect,
						HasItemOverrideColor(iDrawPos,LBC_TEXTHIGHLIGHT) ? GetItemOverrideColor(iDrawPos,LBC_TEXTHIGHLIGHT) : GetItemDefaultColor(LBC_TEXTHIGHLIGHT),
						false,true,&clientclip);
				}
				else
				{
					m_pFont->Draw(m_Items[iDrawPos].strText.c_str(),textrect,
						HasItemOverrideColor(iDrawPos,LBC_TEXT) ? GetItemOverrideColor(iDrawPos,LBC_TEXT) : GetItemDefaultColor(LBC_TEXT),
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

uint32 ConsoleList::AddItem(const char *szText)
{
	return AddItem(szText,-1);
}

uint32 ConsoleList::AddItem(const char *szText, int iIcon)
{
	uint32 dwOut=0;

	if(m_Items.size() < m_iLineCount)
	{
		ListItem i;
		i.strText=szText;
		i.iIcon=iIcon;

		m_Items.push_back(i);
		dwOut=m_Items.size()-1;
	}
	else
	{
		m_iOutPos++;
		if(m_iOutPos >= m_Items.size())
			m_iOutPos=0;

		m_Items[m_iOutPos].strText=szText;
		dwOut=(uint32)m_iOutPos;
	}

	SetSelected(m_Items.size()-1);

	RecalculateItemHeight();
	RecalculateItemWidth(iIcon);

	return dwOut;
}

void ConsoleList::ClearList()
{
	m_iOutPos=0;

	ListBox::ClearList();
}

}

#endif