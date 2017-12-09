
#ifdef DEDEV

#ifndef _CONSOLE_H_

#define _CONSOLE_H_

#include "../GUI/Dialog.h"
#include "../Core/TSingleton.h"

namespace GUI
{
	class ConsoleList;
}

namespace Dev
{

class ConsoleApp : public GUI::Dialog, public Core::TSingleton<ConsoleApp>
{
	public:

		ConsoleApp() : m_pList(0) {}
		~ConsoleApp();

		void OnCreate();

		void OnClick(const uint32 dwID);
		void OnEditBoxEnter(const uint32 dwID);

		void Output(const char *szText,...);

	private:

		GUI::ConsoleList *m_pList;

};

extern bool g_bConsoleCreated;

}

#define g_Console								Dev::ConsoleApp::GetSingletonRef()
extern bool Dev::g_bConsoleCreated;

#endif

#endif