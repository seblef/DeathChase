
#ifndef _WINDOWAPP_H_

#define _WINDOWAPP_H_

#include "ITask.h"
#include "TSingleton.h"
#include <windows.h>

namespace Core
{

class WindowApp : public ITask, public TSingleton<WindowApp>
{
	public:
	
		WindowApp(const int iPriority, HINSTANCE hInst);
		~WindowApp();
		
		bool Initialize();
		bool Stop();
		
		bool Update();
		
		bool OnSuspend();
		bool OnResume();
		
		HWND GethWnd() { return m_hWnd; };

		void SetCursorEnable(bool bEnable) { m_bCursorEnable=bEnable; };
		bool IsCursorEnable() const { return m_bCursorEnable; };

		void SetCursorPos_(int x, int y);
		void GetCursorPos_(int& x, int& y);

		void Resize(int width, int height);
		
	private:
	
		HWND m_hWnd;
		HINSTANCE m_hInst;

		bool m_bCursorEnable;
		int m_iCursorOffset;
		bool m_bWindowed;
		
};

#define g_WindowApp			Core::WindowApp::GetSingletonRef()

}

#endif
