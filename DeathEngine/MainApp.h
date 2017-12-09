
#ifndef _MAINAPP_H_

#define _MAINAPP_H_

#include <windows.h>

class MainApp
{
	public:

		MainApp() {};
		~MainApp() {};

		bool InitializeServices(HINSTANCE hInst);
		bool InitializeTasks(HINSTANCE hInst);

		void Run();

		void Shutdown();

};

#endif