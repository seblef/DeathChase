
#include "SpinBox.h"
#include "EditBox.h"
#include "Button.h"
#include "IEnvironment.h"
#include "../ConvertChar.h"

namespace GUI
{

SpinBox::SpinBox(const char *szText, IEnvironment *pEnvironment, IElement *pParent, 
				 IEventReceiver *pUser, uint32 dwID, const Core::Rect &rect):
	ISpinBox(pEnvironment,pParent,pUser,dwID,rect),
		m_pEditBox(0),m_pSpinUp(0),m_pSpinDown(0),m_fStepSize(1.0f),
		m_fRangeMin(-1e23f),m_fRangeMax(1e23f),m_strFormat("%f"),
		m_iDecimalPlaces(-1)
{
	int buttonwidth=16;
	ISpriteBank *sb=0;

	if(pEnvironment && pEnvironment->GetSkin())
	{
		buttonwidth=pEnvironment->GetSkin()->GetSize(DS_SCROLLBARSIZE);
		sb=pEnvironment->GetSkin()->GetSpriteBank();
	}

	m_pSpinDown=pEnvironment->AddButton(Core::Rect(rect.GetWidth() - buttonwidth,
													 rect.GetHeight() /2 +1,
													 rect.GetWidth(), rect.GetHeight()),this);
	m_pSpinDown->SetSubElement(true);
	m_pSpinDown->SetTabStop(false);
	m_pSpinDown->SetAlignment(AL_LOWERRIGHT,AL_LOWERRIGHT,AL_CENTER,AL_LOWERRIGHT);

	m_pSpinUp=pEnvironment->AddButton(Core::Rect(rect.GetWidth() - buttonwidth, 0,
												   rect.GetWidth(), rect.GetHeight() /2),this);
	m_pSpinUp->SetSubElement(true);
	m_pSpinUp->SetTabStop(false);
	m_pSpinUp->SetAlignment(AL_LOWERRIGHT,AL_LOWERRIGHT,AL_UPPERLEFT,AL_CENTER);

	if(sb)
	{
		ISkin *skin=pEnvironment->GetSkin();
		m_pSpinDown->SetSpriteBank(sb);
		m_pSpinDown->SetSprite(BS_UP,skin->GetIcon(DI_SMALLCURSORDOWN),skin->GetColor(DC_WINDOWSYMBOL));
		m_pSpinDown->SetSprite(BS_DOWN,skin->GetIcon(DI_SMALLCURSORDOWN),skin->GetColor(DC_WINDOWSYMBOL));
		
		m_pSpinUp->SetSpriteBank(sb);
		m_pSpinUp->SetSprite(BS_UP,skin->GetIcon(DI_SMALLCURSORUP),skin->GetColor(DC_WINDOWSYMBOL));
		m_pSpinUp->SetSprite(BS_DOWN,skin->GetIcon(DI_SMALLCURSORUP),skin->GetColor(DC_WINDOWSYMBOL));
	}
	else
	{
		m_pSpinDown->SetText("-");
		m_pSpinUp->SetText("+");
	}

	const Core::Rect rectedit(0,0,rect.GetWidth() - buttonwidth-1, rect.GetHeight());
	m_pEditBox=pEnvironment->AddEditBox(szText,rectedit,true,this,0,-1);
	m_pEditBox->SetSubElement(true);
	m_pEditBox->SetAlignment(AL_UPPERLEFT,AL_LOWERRIGHT,AL_UPPERLEFT,AL_LOWERRIGHT);
}

SpinBox::~SpinBox()
{
	RemoveChild(m_pSpinUp,true);
	RemoveChild(m_pSpinDown,true);
	RemoveChild(m_pEditBox,true);
}

void SpinBox::Reset(const char *szText, IEnvironment *pEnvironment, IElement *pParent, 
				 IEventReceiver *pUser, uint32 dwID, const Core::Rect &rect)
{
	IElement::Reset(pEnvironment,pParent,pUser,dwID,rect);

	int buttonwidth=16;
	ISpriteBank *sb=0;

	if(pEnvironment && pEnvironment->GetSkin())
	{
		buttonwidth=pEnvironment->GetSkin()->GetSize(DS_SCROLLBARSIZE);
		sb=pEnvironment->GetSkin()->GetSpriteBank();
	}

	m_pSpinDown=pEnvironment->AddButton(Core::Rect(rect.GetWidth() - buttonwidth,
													 rect.GetHeight() /2 +1,
													 rect.GetWidth(), rect.GetHeight()),this);
	m_pSpinDown->SetSubElement(true);
	m_pSpinDown->SetTabStop(false);
	m_pSpinDown->SetAlignment(AL_LOWERRIGHT,AL_LOWERRIGHT,AL_CENTER,AL_LOWERRIGHT);

	m_pSpinUp=pEnvironment->AddButton(Core::Rect(rect.GetWidth() - buttonwidth, 0,
												   rect.GetWidth(), rect.GetHeight() /2),this);
	m_pSpinUp->SetSubElement(true);
	m_pSpinUp->SetTabStop(false);
	m_pSpinUp->SetAlignment(AL_LOWERRIGHT,AL_LOWERRIGHT,AL_UPPERLEFT,AL_CENTER);

	if(sb)
	{
		ISkin *skin=pEnvironment->GetSkin();
		m_pSpinDown->SetSpriteBank(sb);
		m_pSpinDown->SetSprite(BS_UP,skin->GetIcon(DI_SMALLCURSORDOWN),skin->GetColor(DC_WINDOWSYMBOL));
		m_pSpinDown->SetSprite(BS_DOWN,skin->GetIcon(DI_SMALLCURSORDOWN),skin->GetColor(DC_WINDOWSYMBOL));
		
		m_pSpinUp->SetSpriteBank(sb);
		m_pSpinUp->SetSprite(BS_UP,skin->GetIcon(DI_SMALLCURSORUP),skin->GetColor(DC_WINDOWSYMBOL));
		m_pSpinUp->SetSprite(BS_DOWN,skin->GetIcon(DI_SMALLCURSORUP),skin->GetColor(DC_WINDOWSYMBOL));
	}
	else
	{
		m_pSpinDown->SetText("-");
		m_pSpinUp->SetText("+");
	}

	const Core::Rect rectedit(0,0,rect.GetWidth() - buttonwidth-1, rect.GetHeight());
	m_pEditBox=pEnvironment->AddEditBox(szText,rectedit,true,this,0,-1);
	m_pEditBox->SetSubElement(true);
	m_pEditBox->SetAlignment(AL_UPPERLEFT,AL_LOWERRIGHT,AL_UPPERLEFT,AL_LOWERRIGHT);
}

void SpinBox::Clear()
{
	RemoveChild(m_pSpinUp,true);
	RemoveChild(m_pSpinDown,true);
	RemoveChild(m_pEditBox,true);

	m_pEditBox=0;
	m_pSpinUp=0;
	m_pSpinDown=0;
	m_fStepSize=1.0f;
	m_fRangeMin=-1e23f;
	m_fRangeMax= 1e23f;
	m_strFormat="%f";
	m_iDecimalPlaces=-1;

	IElement::Clear();
}

IEditBox *SpinBox::GetEditBox_() const
{
	return m_pEditBox;
}

void SpinBox::SetValue(float val)
{
	VerifyValueRange(val);
	char str[128];
	sprintf(str,m_strFormat.c_str(),val);
	m_pEditBox->SetText(str);
}

float SpinBox::GetValue() const
{
	const char *val=m_pEditBox->GetText();
	if(!val)
		return 0.0f;

	return atof(val);
}

void SpinBox::SetRange(float min, float max)
{
	m_fRangeMin=min;
	m_fRangeMax=max;
	SetValue(GetValue());
}

float SpinBox::GetMin() const
{
	return m_fRangeMin;
}

float SpinBox::GetMax() const
{
	return m_fRangeMax;
}

float SpinBox::GetStepSize() const
{
	return m_fStepSize;
}

void SpinBox::SetStepSize(float step)
{
	m_fStepSize=step;
}

void SpinBox::SetDecimalPlaces(int places)
{
	m_iDecimalPlaces=places;
	if(places==-1)
		m_strFormat="%f";
	else
	{
		m_strFormat="%.";
		m_strFormat+=IntToChar(places);
		m_strFormat+="f";
	}
	SetValue(GetValue());
}

bool SpinBox::OnEvent(const Event& _Event)
{
	bool changeevent=false;
	switch(_Event.EventType)
	{
		case ET_GUI:
			if(_Event.sGUI.EventType==GE_BUTTONCLICKED)
			{
				if(_Event.sGUI.pCaller==m_pSpinUp)
				{
					float val=GetValue();
					val+=m_fStepSize;
					SetValue(val);
					changeevent=true;
				}
				else if(_Event.sGUI.pCaller==m_pSpinDown)
				{
					float val=GetValue();
					val-=m_fStepSize;
					SetValue(val);
					changeevent=true;
				}
			}

			if(_Event.sGUI.EventType==GE_EDITBOXENTER)
			{
				if(_Event.sGUI.pCaller==m_pEditBox)
				{
					SetValue(GetValue());
					changeevent=true;
				}
			}
			break;

		default:
			break;
	}

	if(changeevent)
	{
		Event e;
		e.EventType=ET_GUI;
		e.sGUI.pCaller=this;
		e.sGUI.pElement=0;
		e.sGUI.EventType=GE_SPINBOXCHANGED;
		SendEvent(e);

		return true;
	}

	return IElement::OnEvent(_Event);
}

void SpinBox::VerifyValueRange(float &val)
{
	if(val < m_fRangeMin) val=m_fRangeMin;
	else if(val > m_fRangeMax) val=m_fRangeMax;
	else
		return;
}

void SpinBox::SetText(const char *szText)
{
	m_pEditBox->SetText(szText);
	SetValue(GetValue());
}

const char *SpinBox::GetText() const
{
	return m_pEditBox->GetText();
}

}
