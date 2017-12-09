
#include "MultiListBox.h"
#include "IScrollBar.h"
#include "IEnvironment.h"
#include "ISkin.h"

namespace GUI
{

MultiListBox::MultiListBox(int iColumnCount, int *pCWidth,
						   IEnvironment *pEnvironment, IElement *pParent,
						   IEventReceiver *pUser, uint32 dwID, Core::Rect& rect) :
	IMultiListBox(pEnvironment,pParent,pUser,dwID,rect),
		m_iColumnCount(iColumnCount),m_iItemHeight(0),
		m_iTotalItemHeight(0),m_pFont(0),m_pScrollBar(0),
		m_bAutoScroll(true)
{
	MYASSERT(m_iColumnCount > 1,"Nombre de colonne du multilist box invalide",true);
	MYASSERT(pCWidth,"Mauvais pointeur de la liste de taille des colonnes du multilistbox",true);

	int i;
	for(i=0;i<iColumnCount;i++)
		m_ColumnsWidth.push_back(pCWidth[i]);

	ISkin *skin=m_pEnvironment->GetSkin();
	const int s=skin->GetSize(DS_SCROLLBARSIZE);

	m_pScrollBar=m_pEnvironment->AddScrollBar(false,
		Core::Rect(m_RelativeRect.GetWidth()-s,0,m_RelativeRect.GetWidth(),m_RelativeRect.GetHeight()),
		this,0,INVALID_ID);
	m_pScrollBar->SetSubElement(true);
	m_pScrollBar->SetTabStop(false);
	m_pScrollBar->SetAlignment(AL_LOWERRIGHT,AL_LOWERRIGHT,AL_UPPERLEFT,AL_LOWERRIGHT);
	m_pScrollBar->SetVisible(false);
	m_pScrollBar->SetPos(0);
	BringToFront(m_pScrollBar);

	SetTabStop(true);
	SetTabOrder(-1);

	UpdateAbsolutePosition();
}

MultiListBox::~MultiListBox()
{
	RemoveChild(m_pScrollBar,true);
	if(m_pFont) m_pFont->Drop();
}

void MultiListBox::ClearList()
{
	m_Items.clear();

	if(m_pScrollBar)
		m_pScrollBar->SetPos(0);

	RecalculateItemHeight();
}

void MultiListBox::RecalculateItemHeight()
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
	m_pScrollBar->SetMax(m_iTotalItemHeight - m_AbsoluteRect.GetHeight());
	if(m_iTotalItemHeight <= m_AbsoluteRect.GetHeight())
		m_pScrollBar->SetVisible(false);
	else
		m_pScrollBar->SetVisible(true);
}

bool MultiListBox::OnEvent(const Event& _Event)
{
	switch(_Event.EventType)
	{
		case ET_GUI:
			switch(_Event.sGUI.EventType)
			{
				case GE_SCROLLBARCHANGED:
					if(_Event.sGUI.pCaller==m_pScrollBar)
						return true;
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
					case ME_LMOUSEUP:
						return true;

					default:
						break;
				}
			}
			break;
	}

	return m_pParent ? m_pParent->OnEvent(_Event) : false;
}

void MultiListBox::UpdateAbsolutePosition()
{
	IElement::UpdateAbsolutePosition();
	RecalculateItemHeight();
}

void MultiListBox::Draw()
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

	skin->Draw3DSunkenPane(this,skin->GetColor(DC_3DHIGHLIGHT),true,true,framerect,&clientclip);

	if(cliprect)
		clientclip.ClipAgainst(*cliprect);

	framerect=m_AbsoluteRect;
	framerect.m_vUpperLeft.x+=1;
	if(m_pScrollBar->IsVisible())
		framerect.m_vLowerRight.x=m_AbsoluteRect.m_vLowerRight.x - skin->GetSize(DS_SCROLLBARSIZE);

	framerect.m_vLowerRight.y=m_AbsoluteRect.m_vUpperLeft.y + m_iItemHeight;
	framerect.m_vUpperLeft.y-=m_pScrollBar->GetPos();
	framerect.m_vLowerRight.y-=m_pScrollBar->GetPos();

	Core::Rect framerect2(framerect);

	int i;
	int j;

	for(i=0;i<(int)m_Items.size();i++)
	{
		if(framerect.m_vLowerRight.y >= m_AbsoluteRect.m_vUpperLeft.y &&
			framerect.m_vUpperLeft.y <= m_AbsoluteRect.m_vLowerRight.y)
		{
			if(i%2==0)
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
			textrect.m_vLowerRight.x=framerect.m_vUpperLeft.x + m_ColumnsWidth[0];
			
			for(j=0;j<m_iColumnCount;j++)
			{
				if(m_pFont)
				{
					textrect.m_vLowerRight.x=textrect.m_vUpperLeft.x + m_ColumnsWidth[j];
					textrect.m_vUpperLeft.x+=3;

					if(i%2==0)
					{
						m_pFont->Draw(m_Items[i][j].c_str(),textrect,skin->GetColor(DC_HIGHLIGHTTEXT),
							false,true,&clientclip);
					}
					else
					{
						m_pFont->Draw(m_Items[i][j].c_str(),textrect,skin->GetColor(DC_BUTTONTEXT),
							false,true,&clientclip);
					}
				}

				textrect.m_vUpperLeft.x=textrect.m_vLowerRight.x;
			}
		}

		framerect.m_vUpperLeft.y+=m_iItemHeight;
		framerect.m_vLowerRight.y+=m_iItemHeight;
	}

	IElement::Draw();
}

uint32 MultiListBox::AddItem(MultiListBoxVector *pVector)
{
	if(pVector->size()!=m_iColumnCount)
		return m_Items.size()-1;

	m_Items.push_back(*pVector);
	RecalculateItemHeight();

	return m_Items.size() - 1;
}

void MultiListBox::SetAutoScrollEnable(bool bScroll)
{
	m_bAutoScroll=bScroll;
}

bool MultiListBox::IsAutoScrollEnable() const
{
	return m_bAutoScroll;
}

}