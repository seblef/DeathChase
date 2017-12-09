
#include "InOutFader.h"
#include "IEnvironment.h"
#include <windows.h>

namespace GUI
{

InOutFader::InOutFader(IEnvironment *pEnvironment, IElement *pParent, IEventReceiver *pUser,
					   uint32 dwID, Core::Rect rect) :
	IInOutFader(pEnvironment,pParent,pUser,dwID,rect),m_Action(FA_NOTHING),
		m_dwStartTime(0),m_dwEndTime(0)
{
	SetColor(Core::Color32(0,0,0,0));
}

void InOutFader::Reset(IEnvironment *pEnvironment, IElement *pParent, IEventReceiver *pUser,
					   uint32 dwID, Core::Rect rect)
{
	IElement::Reset(pEnvironment,pParent,pUser,dwID,rect);
}

void InOutFader::Clear()
{
	m_Action=FA_NOTHING;
	m_dwStartTime=0;
	m_dwEndTime=0;
	SetColor(Core::Color32(0,0,0,0));

	IElement::Clear();
}

void InOutFader::Draw()
{
	IElement::Draw();
}

Core::Color32 InOutFader::GetColor() const
{
	return m_cColor[1];
}

void InOutFader::SetColor(Core::Color32 cColor)
{
	Core::Color32 s(cColor);
	Core::Color32 d(cColor);

	s.a=255;
	d.a=0;

	SetColor(s,d);
}

void InOutFader::SetColor(Core::Color32 cSource, Core::Color32 cDest)
{
	m_cColor[0]=cSource;
	m_cColor[1]=cDest;

	if(m_Action==FA_FADEOUT)
	{
		m_cFull=m_cColor[1];
		m_cTrans=m_cColor[0];
	}
	else if(m_Action==FA_FADEIN)
	{
		m_cFull=m_cColor[0];
		m_cTrans=m_cColor[1];
	}
}

bool InOutFader::IsReady() const
{
	uint32 now=timeGetTime();
	return (now > m_dwEndTime);
}

void InOutFader::FadeIn(uint32 dwTime)
{
	m_dwStartTime=timeGetTime();
	m_dwEndTime=m_dwStartTime+dwTime;
	m_Action=FA_FADEIN;
	SetColor(m_cColor[0],m_cColor[1]);
}

void InOutFader::FadeOut(uint32 dwTime)
{
	m_dwStartTime=timeGetTime();
	m_dwEndTime=m_dwStartTime+dwTime;
	m_Action=FA_FADEOUT;
	SetColor(m_cColor[0],m_cColor[1]);
}

}