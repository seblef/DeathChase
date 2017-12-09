
#ifdef DEEXPORT
#include "Engine.h"
#include "../Core/FileSystem.h"
#include "../Core/Input.h"
#include "../Core/Timer.h"
#include "../Game/RenderingQueue.h"

#include "../GameTest.h"

#include "../../MyWinMessages.h"


extern "C" __declspec(dllexport) bool GetEngine(IEngine **ppEngine)
{
	*ppEngine=new Engine;

	if(*ppEngine)
		return true;
	else
		return false;
}

extern "C" __declspec(dllexport) bool ReleaseEngine(IEngine **ppEngine)
{
	if(ppEngine && *ppEngine)
	{
		delete *ppEngine;
		*ppEngine=0;
	}

	return true;
}

DWORD g_dwTestThreadID=0;

Engine::Engine() : m_bTesting(false),m_bSuspended(false),m_pShip(0),m_pTest(0),
	m_hWnd(0), m_hInstance(0), m_dwDeltaTime(0)
{
}

Engine::~Engine()
{
}

bool Engine::Start(HINSTANCE hInst)
{
	g_Output->Start("DeathChase.log",true);

	Core::Matrix4::InitTrigo();

#ifdef DEDEV
	new Dev::MemDebugger;
	new Dev::MemAllocMgr;
#endif

	new Core::Timer;
	new Core::Config;
	new Core::FileSystem;
	new Factory;

	bool bRet=g_Config.ParseFile("DeathChase.ini");
	MYASSERT(bRet,"Loading config file",true);

	new Core::Renderer;
	new Game::Map;

	new Core::Input(0,hInst);

	m_hInstance=hInst;

	return true;
}

bool Engine::ShutDown()
{
	if(m_bTesting)
	{
		EndMapTest();
	}

	g_Renderer.Shutdown();
	g_Config.Destroy();
	g_Output->End();

#ifdef DEDEV
	g_MemDebugger.SaveResults();
#endif

	return true;
}

bool Engine::StartMapTest(const char *szSpaceShipName, const char *szPath, DWORD dwThreadID)
{
	g_dwTestThreadID=dwThreadID;

	m_pShip=new Game::SpaceShip;
	if(!m_pShip->Load(szSpaceShipName,szPath))
	{
		delete m_pShip;
		m_pShip=0;
		return false;
	}

	return StartTest();
}

LRESULT CALLBACK TestWndFunc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
     	HDC         hdc;
     	PAINTSTRUCT ps;

	switch (message)
    {
		case WM_PAINT:

			hdc = BeginPaint (hwnd, &ps);
			EndPaint (hwnd, &ps);
			break;

        case WM_DESTROY:
			PostThreadMessage(g_dwTestThreadID,WMSG_KILLTESTTHREAD,0,0);
			break;

		case WM_SYSCOMMAND:

			if((wParam & 0xFFF0)==SC_SCREENSAVE || 
			   (wParam & 0xFFF0)==SC_MONITORPOWER)
				return 0;
			break;

		case WM_SIZE:

			switch(wParam)
			{
				case SIZE_MINIMIZED:
					PostThreadMessage(g_dwTestThreadID,WMSG_SUSPENDTESTTHREAD,0,0);
					break;

				case SIZE_RESTORED:
					PostThreadMessage(g_dwTestThreadID,WMSG_RESUMETESTTHREAD,0,0);
					break;
			}
			break;

		case WM_KILLFOCUS:
			PostThreadMessage(g_dwTestThreadID,WMSG_SUSPENDTESTTHREAD,0,0);
			break;

		case WM_SETFOCUS:
			PostThreadMessage(g_dwTestThreadID,WMSG_RESUMETESTTHREAD,0,0);
			break;
		
		case WM_KEYDOWN:
			if(wParam==VK_ESCAPE)
				PostThreadMessage(g_dwTestThreadID,WMSG_KILLTESTTHREAD,0,0);
			break;
 	}

   	return DefWindowProc (hwnd, message, wParam, lParam);
}

bool Engine::StartTest()
{
     WNDCLASS    wndclass;
     char szAppName[] = "Death Chase Test";

     wndclass.style         = CS_HREDRAW | CS_VREDRAW;
     wndclass.lpfnWndProc   = TestWndFunc;		 // window function
     wndclass.cbClsExtra    = 0;			 // no extra count of bytes
     wndclass.cbWndExtra    = 0;			 // no extra count of bytes
     wndclass.hInstance     = m_hInstance; // this instance
     wndclass.hIcon         = LoadIcon (NULL, IDI_APPLICATION);
     wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW);
     wndclass.hbrBackground = (HBRUSH) GetStockObject (BLACK_BRUSH);
     wndclass.lpszMenuName  = NULL;
     wndclass.lpszClassName = szAppName;

     RegisterClass (&wndclass);

     m_hWnd = CreateWindowEx (
                    WS_EX_APPWINDOW | WS_EX_TOPMOST,         
                    szAppName,             
                    "-= Death Chase Test =-",
                    WS_CAPTION | WS_MINIMIZEBOX | WS_VISIBLE | WS_SYSMENU, 
                    100,         
                    50,         
                    640,                   
                    510,                 
                    NULL,                  
                    NULL,                  
                    m_hInstance,        
                    NULL);              
     
	 if (!m_hWnd)
	 {
		 g_Output->AddMsg("x Failed creating test window\n");
		 return false;
	 }

     ShowWindow (m_hWnd, TRUE);
     UpdateWindow (m_hWnd);
     SetFocus(m_hWnd);

	m_pTest=new GameTest(m_pShip,m_hWnd);
	m_bTesting=m_pTest->Initialize();

	m_bSavedLight=g_RenderingQueue.IsLightEnable();
	m_bSavedShadows=g_RenderingQueue.IsShadowsEnable();

	g_RenderingQueue.SetLightEnable(true);
	g_RenderingQueue.SetShadowsEnable(true);

	g_Renderer.EnableMultiThreading(true);

	return m_bTesting;
}

bool Engine::EndMapTest()
{
	g_Renderer.EnableMultiThreading(false);

	if(m_pTest)
	{
		m_pTest->Stop();
		delete m_pTest;
	}

	if(m_pShip) delete m_pShip;

	m_pTest=0;
	m_pShip=0;

	DestroyWindow(m_hWnd);

	m_bTesting=false;
	m_bSuspended=false;

	g_RenderingQueue.SetLightEnable(m_bSavedLight);
	g_RenderingQueue.SetShadowsEnable(m_bSavedShadows);

	return true;
}

bool Engine::SuspendTest()
{
	if(!m_bSuspended)
	{
		g_Input.OnSuspend();
		m_bSuspended=true;
		m_dwSuspendTime=g_Timer.GetTimeInMicroS();

		g_Renderer.EnableMultiThreading(false);
	}

	g_RenderingQueue.SetLightEnable(m_bSavedLight);
	g_RenderingQueue.SetShadowsEnable(m_bSavedShadows);

	return true;
}

bool Engine::ResumeTest()
{
	if(m_bSuspended)
	{
		g_Input.OnResume();
		g_Renderer.EnableMultiThreading(true);
		m_bSuspended=false;
		m_dwDeltaTime+=g_Timer.GetTimeInMicroS() - m_dwSuspendTime;
	}

	g_RenderingQueue.SetLightEnable(true);
	g_RenderingQueue.SetShadowsEnable(true);

	return true;
}

bool Engine::UpdateTest()
{
	MSG msg;

	if(PeekMessage(&msg, NULL,0,0,PM_NOREMOVE))
	{
		if(!GetMessage(&msg,NULL,0,0))
			return true;

		TranslateMessage (&msg);
		DispatchMessage (&msg);
	}

	if(!m_bSuspended && m_pTest)
		m_pTest->Update(g_Timer.GetTimeInMicroS() - m_dwDeltaTime);

	return true;
}

void Engine::EnableLight(bool bEnable)
{
	g_RenderingQueue.SetLightEnable(bEnable);
}

void Engine::EnableShadows(bool bEnable)
{
	g_RenderingQueue.SetShadowsEnable(bEnable);
}

#endif