
//#ifndef DEEXPORT

#include "MainApp.h"

int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int iCmdShow)
{
	if(hInstance==hPrevInstance)
		return 1;

	MainApp *pApp=new MainApp;

	if(pApp->InitializeServices(hInstance))
	{
		if(pApp->InitializeTasks(hInstance))
			pApp->Run();
	}

	pApp->Shutdown();
	delete pApp;

    return 0;
}

//#endif