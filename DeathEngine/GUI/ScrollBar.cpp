
#include "ScrollBar.h"
#include "ISkin.h"
#include "IEnvironment.h"
#include "Button.h"
#include "IFont.h"
#include <dinput.h>

namespace GUI
{

ScrollBar::ScrollBar(bool bHorizontal, IEnvironment *pEnvironment,
					 IElement *pParent, IEventReceiver *pUser, 
					 uint32 dwID, Core::Rect rect, bool bNoClip) :
	IScrollBar(pEnvironment,pParent,pUser,dwID,rect), m_pUp(0),m_pDown(0),
		m_bDragging(false),m_bHorizontal(bHorizontal),
		m_bDraggedBySlider(false),m_bTrayClick(false),m_iPos(0),m_iDrawPos(0),
		m_iDrawHeight(0),m_iMax(100),m_iSmallStep(10),m_iLargeStep(50),
		m_iDesiredPos(0),m_dwLastChange(0)
{
	RefreshControls();
	SetNoClip(bNoClip);

	SetTabStop(true);
	SetTabOrder(-1);

	SetPos(0);
}

ScrollBar::~ScrollBar()
{
	RemoveChild(m_pUp,true);
	RemoveChild(m_pDown,true);
}

void ScrollBar::Reset(bool bHorizontal, IEnvironment *pEnvironment,
					 IElement *pParent, IEventReceiver *pUser, uint32 dwID, Core::Rect rect, bool bNoClip)
{
	IElement::Reset(pEnvironment,pParent,pUser,dwID,rect);

	m_bHorizontal=bHorizontal;

	RefreshControls();
	SetNoClip(bNoClip);

	SetTabStop(true);
	SetTabOrder(-1);

	SetPos(0);
}

void ScrollBar::Clear()
{
	RemoveChild(m_pUp,true);
	RemoveChild(m_pDown,true);

	m_pUp=0;
	m_pDown=0;
	m_bDragging=false;
	m_bDraggedBySlider=false;
	m_bTrayClick=false;
	m_iPos=0;
	m_iDrawPos=0;
	m_iDrawHeight=0;
	m_iMax=100;
	m_iSmallStep=10;
	m_iLargeStep=50;
	m_iDesiredPos=0;
	m_dwLastChange=0;

	IElement::Clear();
}

bool ScrollBar::OnEvent(const Event& _Event)
{
	switch(_Event.EventType)
	{
		case ET_KEY:
			if(_Event.sKey.bPressed)
			{
				const int oldpos=m_iPos;
				bool absorb=true;

				switch(_Event.sKey.cDXChar)
				{
					case DIK_LEFT:
					case DIK_UP:
						SetPos(m_iPos - m_iSmallStep);
						break;
					case DIK_RIGHT:
					case DIK_DOWN:
						SetPos(m_iPos + m_iSmallStep);
						break;
					case DIK_HOME:
						SetPos(0);
						break;
					case DIK_PRIOR:
						SetPos(m_iPos - m_iLargeStep);
						break;
					case DIK_END:
						SetPos(m_iMax);
						break;
					case DIK_NEXT:
						SetPos(m_iPos + m_iLargeStep);
						break;
					default:
						absorb=false;
				}

				if(m_iPos!=oldpos)
				{
					Event newEvent;
					newEvent.EventType=ET_GUI;
					newEvent.sGUI.pCaller=this;
					newEvent.sGUI.pElement=0;
					newEvent.sGUI.EventType=GE_SCROLLBARCHANGED;
					m_pParent->OnEvent(newEvent);

					if(m_pUserReceiver)
						m_pUserReceiver->OnEvent(newEvent);
				}
				if(absorb)
					return true;
			}
			break;
		case ET_GUI:
			if(_Event.sGUI.EventType==GE_BUTTONCLICKED)
			{
				if(_Event.sGUI.pCaller==m_pUp)
					SetPos(m_iPos - m_iSmallStep);
				else if(_Event.sGUI.pCaller==m_pDown)
					SetPos(m_iPos + m_iSmallStep);

				Event newEvent;
				newEvent.EventType=ET_GUI;
				newEvent.sGUI.pCaller=this;
				newEvent.sGUI.pElement=0;
				newEvent.sGUI.EventType=GE_SCROLLBARCHANGED;
				m_pParent->OnEvent(newEvent);

				if(m_pUserReceiver)
					m_pUserReceiver->OnEvent(newEvent);

				return true;
			}
			else if(_Event.sGUI.EventType==GE_FOCUSLOST)
			{
				if(_Event.sGUI.pCaller==this)
					m_bDragging=false;
			}
			break;
		case ET_MOUSE:
			switch(_Event.sMouse.EventType)
			{
				case ME_MOUSEWHEEL:
					if(m_pEnvironment->HasFocus(this))
					{
						SetPos(GetPos() + (int)_Event.sMouse.fWheel* -m_iSmallStep);
						Event newEvent;
						newEvent.EventType=ET_GUI;
						newEvent.sGUI.pCaller=this;
						newEvent.sGUI.pElement=0;
						newEvent.sGUI.EventType=GE_SCROLLBARCHANGED;
						m_pParent->OnEvent(newEvent);

						if(m_pUserReceiver)
							m_pUserReceiver->OnEvent(newEvent);
						return true;
					}
					break;
				case ME_LMOUSEDOWN:
					{
						if(m_AbsoluteClippingRect.IsPointInside(Core::Vector2i((int)_Event.sMouse.dwX,(int)_Event.sMouse.dwY)))
						{
							m_bDragging=true;
							m_bDraggedBySlider=m_SliderRect.IsPointInside(Core::Vector2i((int)_Event.sMouse.dwX,(int)_Event.sMouse.dwY));
							m_bTrayClick=!m_bDraggedBySlider;
							m_iDesiredPos=GetPosFromMousePos((int)_Event.sMouse.dwX,(int)_Event.sMouse.dwY);
							return true;
						}
						break;
					}
				case ME_LMOUSEUP:
				case ME_MOVED:
					if(m_bDragging)
					{
						if(_Event.sMouse.EventType==ME_LMOUSEUP)
							m_bDragging=false;

						const int newpos=GetPosFromMousePos((int)_Event.sMouse.dwX,(int)_Event.sMouse.dwY);
						const int oldpos=m_iPos;

						if(!m_bDraggedBySlider)
						{
							if(m_AbsoluteClippingRect.IsPointInside(Core::Vector2i((int)_Event.sMouse.dwX,(int)_Event.sMouse.dwY)))
							{
								m_bDraggedBySlider=m_SliderRect.IsPointInside(Core::Vector2i((int)_Event.sMouse.dwX,(int)_Event.sMouse.dwY));
								m_bTrayClick=!m_bDraggedBySlider;
							}
							else
							{
								m_bTrayClick=false;
								if(_Event.sMouse.EventType==ME_MOVED)
									return true;
							}
						}

						if(m_bDraggedBySlider)
							SetPos(newpos);
						else
							m_iDesiredPos=newpos;

						if(m_iPos!=oldpos && m_pParent)
						{
							Event newEvent;
							newEvent.EventType=ET_GUI;
							newEvent.sGUI.pCaller=this;
							newEvent.sGUI.pElement=0;
							newEvent.sGUI.EventType=GE_SCROLLBARCHANGED;
							m_pParent->OnEvent(newEvent);

							if(m_pUserReceiver)
								m_pUserReceiver->OnEvent(newEvent);
						}
						return true;
					}
					break;
				
				default:
					break;
			}
			break;

		default:
			break;
	}

	return IElement::OnEvent(_Event);
}

void ScrollBar::Draw()
{
	if(!m_bIsVisible)
		return;

	ISkin *skin=m_pEnvironment->GetSkin();
	if(!skin)
		return;

	uint32 now=timeGetTime();

	if(m_bDragging && !m_bDraggedBySlider && m_bTrayClick && now > m_dwLastChange + 200)
	{
		m_dwLastChange=now;
		const int oldpos=m_iPos;

		if(m_iDesiredPos >= m_iPos + m_iLargeStep)
			SetPos(m_iPos + m_iLargeStep);
		else if(m_iDesiredPos <= m_iPos - m_iLargeStep)
			SetPos(m_iPos - m_iLargeStep);
		else if(m_iDesiredPos >= m_iPos - m_iLargeStep && m_iDesiredPos <= m_iPos + m_iLargeStep)
			SetPos(m_iDesiredPos);

		if(m_iPos!=oldpos && m_pParent)
		{
			Event newEvent;
			newEvent.EventType=ET_GUI;
			newEvent.sGUI.pCaller=this;
			newEvent.sGUI.pElement=0;
			newEvent.sGUI.EventType=GE_SCROLLBARCHANGED;
			m_pParent->OnEvent(newEvent);

			if(m_pUserReceiver)
				m_pUserReceiver->OnEvent(newEvent);
		}
	}

	m_SliderRect=m_AbsoluteRect;
	skin->Draw2DRectangle(this,skin->GetColor(DC_SCROLLBAR),m_SliderRect,&m_AbsoluteClippingRect);

	if(m_iMax!=0)
	{
		if(m_bHorizontal)
		{
			m_SliderRect.m_vUpperLeft.x=m_AbsoluteRect.m_vUpperLeft.x + m_iDrawPos + m_RelativeRect.GetHeight() - m_iDrawHeight / 2;
			m_SliderRect.m_vLowerRight.x=m_SliderRect.m_vUpperLeft.x + m_iDrawHeight;
		}
		else
		{
			m_SliderRect.m_vUpperLeft.y=m_AbsoluteRect.m_vUpperLeft.y + m_iDrawPos + m_RelativeRect.GetWidth() - m_iDrawHeight / 2;
			m_SliderRect.m_vLowerRight.y=m_SliderRect.m_vUpperLeft.y + m_iDrawHeight;
		}

		skin->Draw3DButtonPaneStandard(this,m_SliderRect,&m_AbsoluteClippingRect);
	}

	IElement::Draw();
}

void ScrollBar::UpdateAbsolutePosition()
{
	IElement::UpdateAbsolutePosition();
	RefreshControls();

	if(m_bHorizontal)
	{
		float f=0.0f;
		if(m_iMax!=0)
			f=((float)m_RelativeRect.GetWidth() - ((float)m_RelativeRect.GetHeight() * 3.0f)) / (float)m_iMax;

		m_iDrawPos=(int)(((float)m_iPos * f) + ((float)m_RelativeRect.GetHeight() * 0.5f));
		m_iDrawHeight=m_RelativeRect.GetHeight();
	}
	else
	{
		float f=0.0f;
		if(m_iMax!=0)
			f=((float)m_RelativeRect.GetHeight() - ((float)m_RelativeRect.GetWidth() * 3.0f)) / (float)m_iMax;

		m_iDrawPos=(int)(((float)m_iPos * f) + ((float)m_RelativeRect.GetWidth() * 0.5f));
		m_iDrawHeight=m_RelativeRect.GetWidth();
	}
}

int ScrollBar::GetPosFromMousePos(int x, int y) const
{
	if(m_bHorizontal)
	{
		const float w=(float)m_RelativeRect.GetWidth() - (float)m_RelativeRect.GetHeight() * 3.0f;
		const float p=(float)(x - m_AbsoluteRect.m_vUpperLeft.x) - (float)m_RelativeRect.GetHeight() * 1.5f;
		return (int)(p/w * (float)m_iMax);
	}
	else
	{
		const float h=(float)m_RelativeRect.GetHeight() - (float)m_RelativeRect.GetWidth() * 3.0f;
		const float p=(float)(y - m_AbsoluteRect.m_vUpperLeft.y) - (float)m_RelativeRect.GetWidth() * 1.5f;
		return (int)(p/h * (float)m_iMax);
	}
}

void ScrollBar::SetPos(int pos)
{
	if(pos < 0) m_iPos=0;
	else if(pos > m_iMax) m_iPos=m_iMax;
	else m_iPos=pos;

	if(m_bHorizontal)
	{
		float f=0.0f;
		if(m_iMax!=0)
			f=((float)m_RelativeRect.GetWidth() - ((float)m_RelativeRect.GetHeight() * 3.0f)) / (float)m_iMax;

		m_iDrawPos=(int)(((float)m_iPos * f) + ((float)m_RelativeRect.GetHeight() * 0.5f));
		m_iDrawHeight=m_RelativeRect.GetHeight();
	}
	else
	{
		float f=0.0;
		if(m_iMax!=0)
			f=((float)m_RelativeRect.GetHeight() - ((float)m_RelativeRect.GetWidth() * 3.0f)) / (float)m_iMax;

		m_iDrawPos=(int)(((float)m_iPos * f) + ((float)m_RelativeRect.GetWidth() * 0.5f));
		m_iDrawHeight=m_RelativeRect.GetWidth();
	}
}

int ScrollBar::GetSmallStep() const
{
	return m_iSmallStep;
}

void ScrollBar::SetSmallStep(int iStep)
{
	if(iStep > 0) m_iSmallStep=iStep;
	else m_iSmallStep=10;
}

int ScrollBar::GetLargeStep() const
{
	return m_iLargeStep;
}

void ScrollBar::SetLargeStep(int iStep)
{
	if(iStep > 0) m_iLargeStep=iStep;
	else m_iLargeStep=50;
}

int ScrollBar::GetMax() const
{
	return m_iMax;
}

void ScrollBar::SetMax(int iMax)
{
	if(iMax > 0) m_iMax=iMax;
	else m_iMax=0;

	bool enable=(m_iMax!=0);
	m_pUp->SetEnable(enable);
	m_pDown->SetEnable(enable);
	SetPos(m_iPos);
}

int ScrollBar::GetPos() const
{
	return m_iPos;
}

void ScrollBar::RefreshControls()
{
	Core::Color32 color(255,255,255,255);

	ISkin *skin=m_pEnvironment->GetSkin();
	ISpriteBank *sprites=0;

	if(skin)
	{
		sprites=skin->GetSpriteBank();
		color=skin->GetColor(DC_WINDOWSYMBOL);
	}

	if(m_bHorizontal)
	{
		int h=m_RelativeRect.GetHeight();
		if(!m_pUp)
		{
			m_pUp=m_pEnvironment->AddButton(Core::Rect(0,0,h,h),this,0,INVALID_ID);
			m_pUp->SetNoClip(m_bNoClip);
			m_pUp->SetSubElement(true);
			m_pUp->SetTabStop(false);
		}
		if(sprites)
		{
			m_pUp->SetSpriteBank(sprites);
			m_pUp->SetSprite(BS_UP,skin->GetIcon(DI_CURSORLEFT),color);
			m_pUp->SetSprite(BS_DOWN,skin->GetIcon(DI_CURSORLEFT),color);
		}

		m_pUp->SetRelativePosition(Core::Rect(0,0,h,h));
		m_pUp->SetAlignment(AL_UPPERLEFT,AL_UPPERLEFT,AL_UPPERLEFT,AL_LOWERRIGHT);

		if(!m_pDown)
		{
			m_pDown=m_pEnvironment->AddButton(Core::Rect(m_RelativeRect.GetWidth()-h,0,m_RelativeRect.GetWidth(),h),this,0,INVALID_ID,0);
			m_pDown->SetNoClip(m_bNoClip);
			m_pDown->SetSubElement(true);
			m_pDown->SetTabStop(false);
		}
		if(sprites)
		{
			m_pDown->SetSpriteBank(sprites);
			m_pDown->SetSprite(BS_UP,skin->GetIcon(DI_CURSORRIGHT),color);
			m_pDown->SetSprite(BS_DOWN,skin->GetIcon(DI_CURSORRIGHT),color);
		}

		m_pDown->SetRelativePosition(Core::Rect(m_RelativeRect.GetWidth()-h,0,m_RelativeRect.GetWidth(),h));
		m_pDown->SetAlignment(AL_LOWERRIGHT,AL_LOWERRIGHT,AL_UPPERLEFT,AL_LOWERRIGHT);
	}
	else
	{
		int w=m_RelativeRect.GetWidth();

		if(!m_pUp)
		{
			m_pUp=m_pEnvironment->AddButton(Core::Rect(0,0,w,w),this,0,INVALID_ID);
			m_pUp->SetNoClip(m_bNoClip);
			m_pUp->SetSubElement(true);
			m_pUp->SetTabStop(false);
		}
		if(sprites)
		{
			m_pUp->SetSpriteBank(sprites);
			m_pUp->SetSprite(BS_UP,skin->GetIcon(DI_CURSORUP),color);
			m_pUp->SetSprite(BS_DOWN,skin->GetIcon(DI_CURSORUP),color);
		}

		m_pUp->SetRelativePosition(Core::Rect(0,0,w,w));
		m_pUp->SetAlignment(AL_UPPERLEFT,AL_LOWERRIGHT,AL_UPPERLEFT,AL_UPPERLEFT);

		if(!m_pDown)
		{
			m_pDown=m_pEnvironment->AddButton(Core::Rect(0,m_RelativeRect.GetHeight()-w,w,m_RelativeRect.GetHeight()),this,0,INVALID_ID);
			m_pDown->SetNoClip(m_bNoClip);
			m_pDown->SetSubElement(true);
			m_pDown->SetTabStop(false);
		}
		if(sprites)
		{
			m_pDown->SetSpriteBank(sprites);
			m_pDown->SetSprite(BS_UP,skin->GetIcon(DI_CURSORDOWN),color);
			m_pDown->SetSprite(BS_DOWN,skin->GetIcon(DI_CURSORDOWN),color);
		}

		m_pDown->SetRelativePosition(Core::Rect(0,m_RelativeRect.GetHeight()-w,w,m_RelativeRect.GetHeight()));
		m_pDown->SetAlignment(AL_UPPERLEFT,AL_LOWERRIGHT,AL_LOWERRIGHT,AL_LOWERRIGHT);
	}
}

}