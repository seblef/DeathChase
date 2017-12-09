
#ifndef _INPUT_H_

#define _INPUT_H_

#include "ITask.h"
#include "TSingleton.h"
#include "../Common.h"
#define DIRECTINPUT_VERSION 0x0700
#include <dinput.h>

namespace Core
{

class Input : public ITask, public TSingleton<Input>
{
	public:
	
		typedef struct _MouseState
		{
			int iX;
			int iY;
			int iWheel;
			bool bButtons[3];
			bool bLastButtons[3];

		} MouseState;

		typedef struct _KeyboardState
		{
			bool bKeyDown;
			char cKeys[256];
			char cLastKeys[256];

		} KeyboardState;

		Input(const int iPriority, HINSTANCE hInst);
		~Input();

#ifdef DEEXPORT
		bool Initialize(HWND hWnd);
#endif
		bool Initialize();

		bool Stop();
		
		bool Update();
		
		bool OnSuspend();
		bool OnResume();
		
		void GetMouseState(MouseState& MState);
		void GetKeyboardState(KeyboardState& KState);
		
		char ConvertDIKToASCII(const byte _DIK) const;
		const char *GetKeyName(const byte _DIK) const;

		void EnableMouseClip(const bool bEnable, int x1, int y1, int x2, int y2);
		
	private:
	
		LPDIRECTINPUT m_pDI;
		LPDIRECTINPUTDEVICE m_pMDevice;
		LPDIRECTINPUTDEVICE m_pKDevice;

		HINSTANCE m_hInst;
		HWND m_hWnd;

		MouseState m_CurrentMState;
		KeyboardState m_CurrentKState;

		bool m_bClip;
		int m_iLeft,m_iUp;
		int m_iRight,m_iDown;

		bool m_bWindowed;

};

}

#define g_Input		Core::Input::GetSingletonRef()

#endif