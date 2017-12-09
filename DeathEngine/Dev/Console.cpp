
#ifdef DEDEV

namespace Dev
{
	bool g_bConsoleCreated=false;
}

#include "Console.h"
#include "../GUI/IEditBox.h"
#include "../GUI/ConsoleList.h"
#include "../GUI/IWindow.h"
#include <stdarg.h>


#define ID_CONSOLEOUTLIST		101
#define ID_CONSOLECLEARBTN		102
#define ID_CONSOLEINEDIT		103

namespace Dev
{

ConsoleApp::~ConsoleApp()
{
	if(m_pList)
	{
		RemoveControl((GUI::IElement *)m_pList);
		delete m_pList;
	}

	m_pList=0;

	g_bConsoleCreated=false;
}

void ConsoleApp::OnCreate()
{
	g_bConsoleCreated=true;

	Core::Rect winrect(m_pWindow->GetAbsolutePosition());

	if(!m_pList)
	{
		m_pList=new GUI::ConsoleList(128,m_pEnvironment,(GUI::IElement *)m_pWindow,this,
			ID_CONSOLEOUTLIST,
			Core::Rect(5,20,winrect.GetWidth()-10,winrect.GetHeight() - 50));

		AddControl((GUI::IElement *)m_pList);
	}
}

void ConsoleApp::OnClick(const uint32 dwID)
{
	if(dwID==ID_CONSOLECLEARBTN && m_pList)
		m_pList->ClearList();
}

void ConsoleApp::OnEditBoxEnter(const uint32 dwID)
{
	if(dwID==ID_CONSOLEINEDIT)
	{
		GUI::IEditBox *eb=(GUI::IEditBox *)GetElementByID(ID_CONSOLEINEDIT);

		if(m_pList && eb)
		{
			const char *szText=eb->GetText();
			if(strlen(szText) > 0)
				m_pList->AddItem(szText);

			eb->SetText("> ");
		}
	}
}

void ConsoleApp::Output(const char *szText, ...)
{
	va_list args;
	va_start(args,szText);

	if(m_pList)
	{
		static char szTempBuffer[1024];
		vsprintf(szTempBuffer,szText,args);

		int ln=strlen(szTempBuffer);

		static std::string strBuffer;
		strBuffer.clear();

		int i;
		for(i=0;i<ln;i++)
		{
			if(szTempBuffer[i]=='\n')
			{
				m_pList->AddItem(strBuffer.c_str(),-1);
				strBuffer.clear();
			}
			else
				strBuffer+=szTempBuffer[i];
		}

		uint32 dwPos=m_pList->AddItem(strBuffer.c_str(),-1);
	}
}

}

#endif