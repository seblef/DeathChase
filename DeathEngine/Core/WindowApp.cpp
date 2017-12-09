
#include "WindowApp.h"
#include "Kernel.h"
#include "Config.h"


namespace Core
{

bool g_bFocusLost;
LRESULT CALLBACK WndFunc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

WindowApp::WindowApp(const int iPriority, HINSTANCE hInst) : ITask(iPriority), m_hWnd(0),m_hInst(hInst),
	m_bCursorEnable(true),m_iCursorOffset(0),m_bWindowed(true)
{
	strcpy(m_szName,"WindowApp");
}

WindowApp::~WindowApp()
{}

bool WindowApp::Initialize()
{
	if(!m_hInst)
		return false;

	int iWidth=g_Config.GetIParameter("WindowWidth",640);
	int iHeight=g_Config.GetIParameter("WindowHeight",480);
	m_bWindowed=g_Config.GetBParameter("Windowed",true);

     WNDCLASS    wndclass;
     char szAppName[] = "Death Chase 0.1";

     wndclass.style         = CS_HREDRAW | CS_VREDRAW;
     wndclass.lpfnWndProc   = WndFunc;		 // window function
     wndclass.cbClsExtra    = 0;			 // no extra count of bytes
     wndclass.cbWndExtra    = 0;			 // no extra count of bytes
     wndclass.hInstance     = m_hInst; // this instance
     wndclass.hIcon         = LoadIcon (NULL, IDI_APPLICATION);
     wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW);
     wndclass.hbrBackground = (HBRUSH) GetStockObject (BLACK_BRUSH);
     wndclass.lpszMenuName  = NULL;
     wndclass.lpszClassName = szAppName;

     RegisterClass (&wndclass);

     m_hWnd = CreateWindowEx (
                    WS_EX_APPWINDOW,//WS_EX_TOPMOST,         
                    szAppName,             
                    "-= DeathChase 0.1 by Koyot =-",
                    WS_CAPTION | WS_MINIMIZEBOX | WS_VISIBLE | WS_SYSMENU, 
                    100,         
                    0,         
                    iWidth,                   
                    iHeight+30,                 
                    NULL,                  
                    NULL,                  
                    m_hInst,        
                    NULL);              
     
	 if (!m_hWnd)
	 {
		 g_Output->AddMsg("x Failed creating main window\n");
		 return false;
	 }

     ShowWindow (m_hWnd, TRUE);
     UpdateWindow (m_hWnd);
     SetFocus(m_hWnd);

	ShowCursor(m_bCursorEnable ? TRUE : FALSE);

	TITLEBARINFO ti;
	ti.cbSize=sizeof(ti);
	GetTitleBarInfo(m_hWnd,&ti);

	m_iCursorOffset=ti.rcTitleBar.bottom-ti.rcTitleBar.top;

	g_bFocusLost=false;
	return true;
}

bool WindowApp::Stop()
{
	return true;
}

bool WindowApp::OnResume()
{
	return true;
}

bool WindowApp::OnSuspend()
{
	return false;
}

bool WindowApp::Update()
{
	MSG msg;

	if(PeekMessage(&msg, NULL,0,0,PM_NOREMOVE))
	{
		if(!GetMessage(&msg,NULL,0,0))
			return true;

		TranslateMessage (&msg);
		DispatchMessage (&msg);
	}

	return true;
}

LRESULT CALLBACK WndFunc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
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
			g_Kernel.KillAllTasks();
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
					g_Kernel.SuspendAllTasks();
					break;

				case SIZE_RESTORED:
					g_Kernel.ResumeAllTasks();
					break;
			}
			break;

		case WM_KILLFOCUS:
			g_bFocusLost=true;
			g_Kernel.GetSingletonRef().SuspendAllTasks();
			break;

		case WM_SETFOCUS:
			if(g_bFocusLost)
			{
				g_bFocusLost=false;
				g_Kernel.ResumeAllTasks();
			}

			break;
		
		case WM_KEYDOWN:
			if(wParam==VK_ESCAPE)
				g_Kernel.KillAllTasks();
			break;

		case WM_SETCURSOR:
			SetCursor(NULL);
			ShowCursor(g_WindowApp.IsCursorEnable() ? TRUE : FALSE);
			break;
 	}

   	return DefWindowProc (hwnd, message, wParam, lParam);
}

void WindowApp::SetCursorPos_(int x, int y)
{
	POINT p;
	p.x=x;
	p.y=y - m_iCursorOffset;

	if(m_bWindowed)
		p.y-=10;

	ClientToScreen(m_hWnd,&p);
	SetCursorPos(p.x,p.y);
}

void WindowApp::GetCursorPos_(int& x, int& y)
{
	POINT p;
	GetCursorPos(&p);
	ScreenToClient(m_hWnd,&p);

	x=p.x;
	y=p.y + m_iCursorOffset;

	if(m_bWindowed)
		y+=10;
}

void WindowApp::Resize(int width, int height)
{
	SetWindowPos(m_hWnd,HWND_TOPMOST,0,0,width,height+30,SWP_NOMOVE);
}

}
	
