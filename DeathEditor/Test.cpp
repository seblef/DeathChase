
#include "Interfaces.h"
#include "Test.h"

#include "../MyWinMessages.h"

CRITICAL_SECTION Sync;

DWORD WINAPI TestThread(LPVOID lpParam)
{
	HANDLE hStartEvent=lpParam;
	WaitForSingleObject(hStartEvent,INFINITE);

	CloseHandle(hStartEvent);

	bool bSuspended=false;
	bool bFinished=false;

	InitializeCriticalSection(&Sync);
	EnterCriticalSection(&Sync);

	while(!bFinished)
	{
		MSG msg;

		if(bSuspended)
		{
			GetMessage(&msg,NULL,0,0);
			
			if(msg.message==WMSG_KILLTESTTHREAD)
				bFinished=true;
			else if(msg.message==WMSG_RESUMETESTTHREAD)
			{
				g_Engine->ResumeTest();
				bSuspended=false;
				EnterCriticalSection(&Sync);
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			while(PeekMessage(&msg,0,0,0,PM_NOREMOVE))
			{
				GetMessage(&msg,0,0,0);
				if(msg.message==WMSG_KILLTESTTHREAD)
					bFinished=true;
				else if(msg.message==WMSG_SUSPENDTESTTHREAD)
				{
					g_Engine->SuspendTest();
					bSuspended=true;
					LeaveCriticalSection(&Sync);
				}
				else
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
			}
		}

		if(!bSuspended && !bFinished)
			g_Engine->UpdateTest();
	}

	LeaveCriticalSection(&Sync);
	DeleteCriticalSection(&Sync);

	g_Engine->EndMapTest();

	return 0;
}
