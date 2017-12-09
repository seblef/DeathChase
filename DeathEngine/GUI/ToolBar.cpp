
#include "ToolBar.h"
#include "ISkin.h"
#include "IEnvironment.h"
#include "../Core/TextureMgr.h"
#include "../Core/Renderer.h"
#include "Button.h"
#include "IFont.h"

namespace GUI
{

ToolBar::ToolBar(IEnvironment *pEnvironment, IElement *pParent,IEventReceiver *pUser,
				 uint32 dwID, Core::Rect rect) :
	IToolBar(pEnvironment,pParent,pUser,dwID,rect), m_iButtonX(5)
{
	int y=0;
	int parentwidth=100;

	if(pParent)
	{
		parentwidth=pParent->GetAbsolutePosition().GetWidth();

		const std::list<IElement*>& children=pParent->GetChildren();
		std::list<IElement*>::const_iterator it=children.begin();

		for(;it!=children.end();it++)
		{
			Core::Rect r((*it)->GetAbsolutePosition());
			if(r.m_vUpperLeft.x==0 && r.m_vUpperLeft.y <= y &&
				r.m_vLowerRight.x==parentwidth)
				y=r.m_vLowerRight.y;
		}
	}

	Core::Rect rr;
	rr.m_vUpperLeft.x=0;
	rr.m_vUpperLeft.y=y;
	int height=m_pEnvironment->GetSkin()->GetSize(DS_MENUHEIGHT);

	rr.m_vLowerRight.x=parentwidth;
	rr.m_vLowerRight.y=rr.m_vUpperLeft.y+height;
	SetRelativePosition(rr);
}

void ToolBar::Reset(IEnvironment *pEnvironment, IElement *pParent, IEventReceiver *pUser,
				 uint32 dwID, Core::Rect rect)
{
	IElement::Reset(pEnvironment,pParent,pUser,dwID,rect);

	int y=0;
	int parentwidth=100;

	if(pParent)
	{
		parentwidth=pParent->GetAbsolutePosition().GetWidth();

		const std::list<IElement*>& children=pParent->GetChildren();
		std::list<IElement*>::const_iterator it=children.begin();

		for(;it!=children.end();it++)
		{
			Core::Rect r((*it)->GetAbsolutePosition());
			if(r.m_vUpperLeft.x==0 && r.m_vUpperLeft.y <= y &&
				r.m_vLowerRight.x==parentwidth)
				y=r.m_vLowerRight.y;
		}
	}

	Core::Rect rr;
	rr.m_vUpperLeft.x=0;
	rr.m_vUpperLeft.y=y;
	int height=m_pEnvironment->GetSkin()->GetSize(DS_MENUHEIGHT);

	rr.m_vLowerRight.x=parentwidth;
	rr.m_vLowerRight.y=rr.m_vUpperLeft.y+height;
	SetRelativePosition(rr);
}

void ToolBar::Clear()
{
	m_iButtonX=5;
	IElement::Clear();
}

bool ToolBar::OnEvent(const Event &_Event)
{
	if(_Event.EventType==ET_MOUSE &&
		_Event.sMouse.EventType==ME_LMOUSEDOWN)
	{
		if(m_AbsoluteClippingRect.IsPointInside(Core::Vector2i((int)_Event.sMouse.dwX,(int)_Event.sMouse.dwY)))
			return true;
	}

	return m_pParent ? m_pParent->OnEvent(_Event) : false;
}

void ToolBar::Draw()
{
	if(!m_bIsVisible)
		return;

	ISkin *skin=m_pEnvironment->GetSkin();
	if(!skin)
		return;

	Core::Rect rect(m_AbsoluteRect);
	Core::Rect *clip=0;

	skin->Draw3DToolBar(this,rect,clip);

	IElement::Draw();
}

void ToolBar::UpdateAbsolutePosition()
{
	if(m_pParent)
	{
		m_DesiredRect.m_vUpperLeft.x=0;
		m_DesiredRect.m_vLowerRight.x=m_pParent->GetAbsolutePosition().GetWidth();
	}

	IElement::UpdateAbsolutePosition();
}

IButton *ToolBar::AddButton(uint32 dwID, const char *szText, const char *szToolTipText,
							Core::HTexture hTex, Core::HTexture hPressedTex,
							bool bIsPush, bool bUseAlphaChannel)
{
	m_iButtonX+=3;

	Core::Rect rect(m_iButtonX,2,0,0);
	if(!hTex.IsNull())
	{
		int x,y;
		g_TextureMgr.GetTextureSize(hTex,x,y);
		rect.m_vLowerRight.x=rect.m_vUpperLeft.x + x + 8;
		rect.m_vLowerRight.y=rect.m_vUpperLeft.y + y + 6;
	}
	else
	{
		rect.m_vLowerRight.y=rect.m_vUpperLeft.y+m_RelativeRect.GetHeight()-4;
		rect.m_vLowerRight.x=rect.m_vUpperLeft.x+m_RelativeRect.GetHeight()-4;
	}

	m_iButtonX+=rect.GetWidth();

	IButton *button=m_pEnvironment->AddButton(rect,this,m_pUserReceiver,INVALID_ID);

	if(szText) button->SetText(szText);
	if(szToolTipText) button->SetToolTipText(szToolTipText);
	if(!hTex.IsNull()) button->SetImage(hTex);
	if(!hPressedTex.IsNull()) button->SetPressedImage(hPressedTex);
	if(bIsPush) button->SetIsPushButton(bIsPush);
	if(bUseAlphaChannel) button->SetUseAlphaChannel(bUseAlphaChannel);
	
	return button;
}

}