
#include "Button.h"
#include "ISkin.h"
#include "IFont.h"
#include "ISpriteBank.h"
#include "../Core/TextureMgr.h"
#include "../Core/Renderer2D.h"
#include <dinput.h>

namespace GUI
{
Button::Button(IEnvironment *pEnvironment,
			   IElement *pParent, IEventReceiver *pUser,
			   uint32 dwID, Core::Rect rect, bool bNoClip) :
	IButton(pEnvironment,pParent,pUser,dwID,rect), m_bPressed(false),
		m_bIsPushButton(false), m_bUseAlphaChannel(false), m_bBorder(true),
		m_dwMouseOverTime(0),m_dwFocusTime(0),m_dwClickTime(0), m_pSpriteBank(0),
		m_pOverrideFont(0)
{
	int i;
	for(i=0;i<BS_COUNT;i++)
		m_ButtonSprites[i].iIndex=-1;

	SetTabStop(true);
	SetTabOrder(-1);
}

Button::~Button()
{
	if(m_pOverrideFont) m_pOverrideFont->Drop();
	if(!m_hTexture.IsNull()) g_TextureMgr.ReleaseTexture(m_hTexture);
	if(!m_hPressedTexture.IsNull()) g_TextureMgr.ReleaseTexture(m_hPressedTexture);
	if(m_pSpriteBank) m_pSpriteBank->Drop();
}

void Button::Reset(IEnvironment *pEnvironment, IElement *pParent, IEventReceiver *pUser,
				   uint32 dwID, Core::Rect rect)
{
	IElement::Reset(pEnvironment,pParent,pUser, dwID,rect);

	m_bPressed=false;
	m_bIsPushButton=false;
	m_bUseAlphaChannel=false;
	m_bBorder=true;
	m_dwMouseOverTime=0;
	m_dwFocusTime=0;
	m_dwClickTime=0;
	m_pSpriteBank=0;
	m_pOverrideFont=0;

	int i;
	for(i=0;i<BS_COUNT;i++)
		m_ButtonSprites[i].iIndex=-1;

	SetTabStop(true);
	SetTabOrder(-1);
}

void Button::Clear()
{
	if(m_pOverrideFont) m_pOverrideFont->Drop();
	if(!m_hTexture.IsNull()) g_TextureMgr.ReleaseTexture(m_hTexture);
	if(!m_hPressedTexture.IsNull()) g_TextureMgr.ReleaseTexture(m_hPressedTexture);
	if(m_pSpriteBank) m_pSpriteBank->Drop();

	IElement::Clear();
}

void Button::SetDrawBorder(bool bBorder)
{
	m_bBorder=bBorder;
}

void Button::SetSpriteBank(ISpriteBank *pBank)
{
	if(pBank) pBank->Grab();
	if(m_pSpriteBank) m_pSpriteBank->Drop();

	m_pSpriteBank=pBank;
}

void Button::SetSprite(BUTTONSTATE _State, int iIndex,
					   Core::Color32 cColor, bool bLoop)
{
	if(m_pSpriteBank)
	{
		m_ButtonSprites[(int)_State].iIndex=iIndex;
		m_ButtonSprites[(int)_State].cColor=cColor;
		m_ButtonSprites[(int)_State].bLoop=bLoop;
	}
	else
		m_ButtonSprites[(int)_State].iIndex=-1;
}

bool Button::OnEvent(const Event& _Event)
{
	if(!m_bIsEnable)
		return m_pParent ? m_pParent->OnEvent(_Event) : false;

	switch(_Event.EventType)
	{
		case ET_KEY:
			if(_Event.sKey.bPressed &&
			   _Event.sKey.cDXChar==DIK_RETURN ||
			   _Event.sKey.cDXChar==DIK_SPACE)
			{
				if(!m_bIsPushButton)
					SetPressed(true);
				else
					SetPressed(!m_bPressed);

				return true;
			}
			if(m_bPressed && !m_bIsPushButton && _Event.sKey.bPressed && _Event.sKey.cDXChar==DIK_ESCAPE)
			{
				SetPressed(false);
				return true;
			}
			else if(!_Event.sKey.bPressed && m_bPressed &&
				(_Event.sKey.cDXChar==DIK_RETURN ||
				 _Event.sKey.cDXChar==DIK_SPACE))
			{
				if(!m_bIsPushButton)
					SetPressed(false);

				if(m_pParent)
				{
					Event newEvent;
					newEvent.EventType=ET_GUI;
					newEvent.sGUI.pCaller=this;
					newEvent.sGUI.pElement=0;
					newEvent.sGUI.EventType=GE_BUTTONCLICKED;
					SendEvent(newEvent);
				}
				return true;
			}
			break;

		case ET_GUI:
			if(_Event.sGUI.EventType==GE_FOCUSLOST)
			{
				if(_Event.sGUI.pCaller==this && !m_bIsPushButton)
					SetPressed(false);
			}
			break;

		case ET_MOUSE:
			if(_Event.sMouse.EventType==ME_LMOUSEDOWN)
			{
				if(m_pEnvironment->HasFocus(this) &&
					!m_AbsoluteClippingRect.IsPointInside(Core::Vector2i((int)_Event.sMouse.dwX,(int)_Event.sMouse.dwY)))
				{
					m_pEnvironment->RemoveFocus(this);
					return false;
				}

				if(!m_bIsPushButton)
					SetPressed(true);

				m_pEnvironment->SetFocus(this);
				return true;
			}
			else if(_Event.sMouse.EventType==ME_LMOUSEUP)
			{
				bool bWasPressed=m_bPressed;

				if(!m_AbsoluteClippingRect.IsPointInside(Core::Vector2i((int)_Event.sMouse.dwX,(int)_Event.sMouse.dwY)))
				{
					if(!m_bIsPushButton)
						SetPressed(false);
					return true;
				}

				if(!m_bIsPushButton)
					SetPressed(false);
				else
					SetPressed(!m_bPressed);

				if((!m_bIsPushButton && bWasPressed && m_pParent) ||
					(m_bIsPushButton && bWasPressed!=m_bPressed))
				{
					Event newEvent;
					newEvent.EventType=ET_GUI;
					newEvent.sGUI.pCaller=this;
					newEvent.sGUI.pElement=0;
					newEvent.sGUI.EventType=GE_BUTTONCLICKED;
					SendEvent(newEvent);
				}

				return true;
			}
			break;

		default:
			break;
	}

	return m_pParent ? m_pParent->OnEvent(_Event) : false;
}

void Button::Draw()
{
	if(!m_bIsVisible)
		return;

	ISkin *skin=m_pEnvironment->GetSkin();
	IFont *font=m_pOverrideFont;

	if(!m_pOverrideFont)
		font=skin->GetFont(DF_BUTTON);

	Core::Rect rect=m_AbsoluteRect;
	Core::Vector2i spritepos(m_AbsoluteRect.GetCenter());

	if(!m_bPressed)
	{
		if(m_bBorder)
			skin->Draw3DButtonPaneStandard(this,rect,&m_AbsoluteClippingRect);

		if(!m_hTexture.IsNull())
		{
			Core::Vector2i pos(m_AbsoluteRect.GetCenter());
			pos.x-=m_ImageRect.GetWidth() / 2;
			pos.y-=m_ImageRect.GetHeight() / 2;

			g_Renderer2D.Draw2DImage(m_hTexture,pos,m_ImageRect,&m_AbsoluteClippingRect,
				Core::Color32(255,255,255,255),m_bUseAlphaChannel);
		}

		if(m_pSpriteBank && m_ButtonSprites[BS_UP].iIndex!=-1)
		{
			m_pSpriteBank->DrawSprite(m_ButtonSprites[BS_UP].iIndex,spritepos,
				&m_AbsoluteClippingRect,m_ButtonSprites[BS_UP].cColor,m_dwClickTime,
				timeGetTime(),m_ButtonSprites[BS_UP].bLoop,true);
		}
	}
	else
	{
		if(m_bBorder)
			skin->Draw3DButtonPanePressed(this,rect,&m_AbsoluteClippingRect);

		if(!m_hPressedTexture.IsNull())
		{
			Core::Vector2i pos(m_AbsoluteRect.GetCenter());
			pos.x-=m_PressedImageRect.GetWidth() / 2;
			pos.y-=m_PressedImageRect.GetHeight() / 2;

			if(m_hTexture==m_hPressedTexture && m_PressedImageRect==m_ImageRect)
			{
				pos.x+=1;
				pos.y+=1;
			}

			g_Renderer2D.Draw2DImage(m_hPressedTexture,pos,m_PressedImageRect,&m_AbsoluteClippingRect,
				Core::Color32(255,255,255,255),m_bUseAlphaChannel);
		}

		if(m_pSpriteBank && m_ButtonSprites[BS_DOWN].iIndex!=-1)
		{
			m_pSpriteBank->DrawSprite(m_ButtonSprites[BS_DOWN].iIndex, spritepos,
				&m_AbsoluteClippingRect, m_ButtonSprites[BS_DOWN].cColor, m_dwClickTime,
				timeGetTime(), m_ButtonSprites[BS_DOWN].bLoop,true);
		}
	}

	if(m_strText.size())
	{
		rect=m_AbsoluteRect;
		if(m_bPressed)
			rect.m_vUpperLeft.y+=2;

		if(font)
			font->Draw(m_strText.c_str(),rect,
			skin->GetColor(m_bIsEnable ? DC_BUTTONTEXT : DC_GRAYTEXT),true,true,&m_AbsoluteClippingRect);
	}

	IElement::Draw();
}

void Button::SetOverrideFont(IFont *pFont)
{
	if(m_pOverrideFont) m_pOverrideFont->Drop();

	m_pOverrideFont=pFont;

	if(m_pOverrideFont) m_pOverrideFont->Grab();
}

void Button::SetImage(Core::HTexture hTex)
{
	if(m_hTexture)
		g_TextureMgr.ReleaseTexture(m_hTexture);

	m_hTexture=hTex;

	if(!m_hTexture.IsNull())
	{
		int x,y;
		g_TextureMgr.GetTextureSize(m_hTexture,x,y);

		m_ImageRect=Core::Rect(Core::Vector2i::NullVector,Core::Vector2i(x,y));
	}

	if(m_hPressedTexture.IsNull())
		SetPressedImage(m_hTexture);
}

void Button::SetImage(Core::HTexture hTex, const Core::Rect& pos)
{
	if(!m_hTexture.IsNull())
		g_TextureMgr.ReleaseTexture(m_hTexture);

	m_hTexture=hTex;
	m_ImageRect=pos;

	if(m_hPressedTexture.IsNull())
		SetPressedImage(m_hTexture,pos);
}

void Button::SetPressedImage(Core::HTexture hTex)
{
	if(!m_hPressedTexture.IsNull())
		g_TextureMgr.ReleaseTexture(m_hPressedTexture);

	m_hPressedTexture=hTex;

	if(!hTex.IsNull())
	{
		int x,y;
		g_TextureMgr.GetTextureSize(hTex,x,y);

		m_PressedImageRect=Core::Rect(Core::Vector2i::NullVector,Core::Vector2i(x,y));
	}
}

void Button::SetPressedImage(Core::HTexture hTex, const Core::Rect& pos)
{
	if(!m_hPressedTexture.IsNull())
		g_TextureMgr.ReleaseTexture(m_hPressedTexture);

	m_hPressedTexture=hTex;
	m_PressedImageRect=pos;
}

void Button::SetIsPushButton(bool bPush)
{
	m_bIsPushButton=bPush;
}

bool Button::IsPressed() const
{
	return m_bPressed;
}

void Button::SetPressed(bool bPressed)
{
	if(m_bPressed!=bPressed)
	{
		m_dwClickTime=timeGetTime();
		m_bPressed=bPressed;
	}
}

bool Button::IsPushButton() const
{
	return m_bIsPushButton;
}

void Button::SetUseAlphaChannel(bool bUseAlpha)
{
	m_bUseAlphaChannel=bUseAlpha;
}

bool Button::IsAlphaChannelUsed() const
{
	return m_bUseAlphaChannel;
}

bool Button::IsDrawingBorder() const
{
	return m_bBorder;
}

}