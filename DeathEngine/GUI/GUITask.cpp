
#include "GUITask.h"
#include "Environment.h"
#include "IContextMenu.h"
#include "../Core/Input.h"
#include "../Core/Renderer2D.h"
#include "../Core/CursorMgr.h"

namespace GUI
{

GUITask::GUITask(const int iPriority) :
	Core::ITask(iPriority), m_pEnvironment(0), iOldMX(0), iOldMY(0), m_bEnable(false),
		m_pMenu(0)
{
	strcpy(m_szName,"GUI Task");
}

GUITask::~GUITask()
{
}

bool GUITask::Initialize()
{
	if(!m_pEnvironment)
	{
		m_pEnvironment=new Environment;
		ISkin *pSkin=m_pEnvironment->LoadSkin("standard");
		m_pEnvironment->SetSkin(pSkin);

		m_pMenu=m_pEnvironment->AddMenu((GUI::IElement *)m_pEnvironment,this,INVALID_ID);
		m_pMenu->SetVisible(false);
	}

	return true;
}

bool GUITask::Stop()
{
	if(m_pEnvironment)
	{
		m_pEnvironment->Clear();
		delete m_pEnvironment;
	}

	m_pEnvironment=0;

	return true;
}

bool GUITask::Update()
{
	Core::Input::MouseState MState;
	Core::Input::KeyboardState KState;

	g_Input.GetKeyboardState(KState);
	g_Input.GetMouseState(MState);

	if(m_bEnable)
	{
		Event e;
		e.EventType=ET_MOUSE;
		e.sMouse.dwX=MState.iX;
		e.sMouse.dwY=MState.iY;

		if(MState.iX!=iOldMX || MState.iY!=iOldMY)
		{
			e.sMouse.EventType=ME_MOVED;
			m_pEnvironment->PostEventFromUser(e);
		}

		if(MState.bButtons[0] && !MState.bLastButtons[0])
		{
			e.sMouse.EventType=ME_LMOUSEDOWN;
			m_pEnvironment->PostEventFromUser(e);
		}

		if(!MState.bButtons[0] && MState.bLastButtons[0])
		{
			e.sMouse.EventType=ME_LMOUSEUP;
			m_pEnvironment->PostEventFromUser(e);
		}

		if(MState.bButtons[1] && !MState.bLastButtons[2])
		{
			e.sMouse.EventType=ME_RMOUSEDOWN;
			m_pEnvironment->PostEventFromUser(e);
		}

		if(!MState.bButtons[1] && MState.bLastButtons[2])
		{
			e.sMouse.EventType=ME_RMOUSEUP;
			m_pEnvironment->PostEventFromUser(e);
		}

		e.EventType=ET_KEY;
		e.sKey.bPressed=false;

		if(KState.cKeys[DIK_LSHIFT] & 0x80 || KState.cKeys[DIK_RSHIFT] & 0x80)
			e.sKey.bShift=true;
		else
			e.sKey.bShift=false;

		if((KState.cKeys[DIK_LCONTROL] & 0x80) || KState.cKeys[DIK_RCONTROL] & 0x80)
			e.sKey.bControl=true;
		else
			e.sKey.bControl=false;

		int i;
		for(i=0;i<256;i++)
		{
			if((KState.cKeys[i] & 0x80) && !(KState.cLastKeys[i] & 0x80))
			{
				e.sKey.bPressed=true;
				e.sKey.cDXChar=i;
				e.sKey.cChar=g_Input.ConvertDIKToASCII(i);
				m_pEnvironment->PostEventFromUser(e);
			}
		}

		g_Renderer2D.Start();
		m_pEnvironment->DrawAll();
		g_CursorMgr.Update();
		g_Renderer2D.End();
	}

	return true;
}

void GUITask::Enable(const bool bEnable)
{
	m_bEnable=bEnable;
	g_CursorMgr.Enable(bEnable);
	g_CursorMgr.SetCursor(CURSOR_SELECT);
}

bool GUITask::OnSuspend()
{
	return true;
}

bool GUITask::OnResume()
{
	return true;
}

bool GUITask::IsCursorIn(const int x, const int y)
{
/*	if(m_pEnvironment && m_pEnvironment->GetElementFromPoint(Core::Vector2i(x,y)))
		return true;
	else
		return false;*/

	return false;
}

void GUITask::ShowMenu(bool bShow)
{
	m_pMenu->SetVisible(bShow);
}

bool GUITask::IsMenuShown() const
{
	return m_pMenu->IsVisible();
}

void GUITask::AddMenuEventReceiver(IEventReceiver *pReceiver)
{
	m_MenuReceivers.push_back(pReceiver);
}

void GUITask::RemoveMenuEventReceiver(IEventReceiver *pReceiver)
{
	std::vector<IEventReceiver *>::iterator i;
	for(i=m_MenuReceivers.begin();i!=m_MenuReceivers.end();i++)
		if((*i)==pReceiver)
		{
			m_MenuReceivers.erase(i);
			break;
		}
}

bool GUITask::OnEvent(const GUI::Event &_Event)
{
	if(_Event.EventType==ET_GUI && _Event.sGUI.EventType==GE_MENUSELECTED)
	{
		std::vector<IEventReceiver *>::iterator i;
		for(i=m_MenuReceivers.begin();i!=m_MenuReceivers.end();i++)
			(*i)->OnEvent(_Event);
	}

	return true;
}

IContextMenu *GUITask::GetMainMenu()
{
	return m_pMenu;
}

}
