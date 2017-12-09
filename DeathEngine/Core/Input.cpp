
#define INITGUID
#include "Input.h"
#include "WindowApp.h"
#include "Config.h"

char DIKToASCIITable[256]={
	' ',' ','1','2','3','4','5','6','7','8','9','0',
	'-','=',' ','	','q','w','e','r','t','y','u','i','o','p',
	'(',')',' ',' ','a','s','d','f','g','h','j','k','l',';','\'',
	'`',' ','\\','z','x','c','v','b','n','m',',','.','/',' ','*',
	' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','7',
	'8','9','-','4','5','6','+','1','2','3','0',',',' ',' ',' ',' ',' ',' ',
	' ',' ',' ',' ',' ',' ','=',' ','@',':','_',' ',' ',' ',' ',' ','\n',' ',
	' ',' ',' ',' ',' ',' ',' ',',','/',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',
	' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ' };
char DIKNameTable[256][24]=
	{ "Echap","1","2","3","4","5","6","7","8","9","0","-","=","Backspace",
	  "Tab","Q","W","E","R","T","Y","U","I","O","P","(",")","Entrée","Control gauche",
	  "A","S","D","F","G","H","J","K","L",";","'","\0","Shift gauche","\\",
	  "Z","X","C","V","B","N","M",",",".","/","Shift droit","*","Alt gauche","Espace",
	  "\0","F1","F2","F3","F4","F5","F6","F7","F8","F9","F10","Verrou num.","Arr. defil",
	  "Num.7","Num.8","Num.9","Num.-","Num.4","Num.5","Num.6","Num.+",
	  "Num.1","Num.2","Num.3","Num.0","Num..","\0","F11","F12","F13","F14","F15",
	  "\0","\0","\0","\0","\0","\0","Num.=","Precedent","@",",","_","\0","Stop","\0","\0",
	  "Suivant","Num.Entrée","Control droit","Silence","Play/Pause","Stop","Volume-","Volume+",
	  "Home(Web)","Num.,","Num./","\0","Alt droit","Pause","Home","Haut","PageUp","Gauche",
	  "Droite","Fin","Bas","PageDown","Insert","Suppr","Win gauche","Win droit","Apps","Power",
	  "Sleep","Wake","Chercher","Favoris","Rafraichir","Stop (Web)","Suivant (Web)","Précédent (Web)",
	  "Mon ordinateur","Mail","Media" };

namespace Core
{

Input::Input(const int iPriority, HINSTANCE hInst) : ITask(iPriority), m_hInst(hInst),
	m_pDI(0),m_pMDevice(0),m_pKDevice(0),m_bClip(false)
{
	m_CurrentMState.iX=0;
	m_CurrentMState.iY=0;
	m_CurrentMState.iWheel=0;

	m_CurrentMState.bButtons[0]=false;
	m_CurrentMState.bButtons[1]=false;
	m_CurrentMState.bButtons[2]=false;

	m_CurrentMState.bLastButtons[0]=false;
	m_CurrentMState.bLastButtons[1]=false;
	m_CurrentMState.bLastButtons[2]=false;

	m_CurrentKState.bKeyDown=false;
	memset(m_CurrentKState.cKeys,0,256);
	memset(m_CurrentKState.cLastKeys,0,256);

	strcpy(m_szName,"Input");
}

Input::~Input()
{
	if(m_pDI)
		m_pDI->Release();
	m_pDI=0;
}

#ifdef DEEXPORT
bool Input::Initialize(HWND hWnd)
{
	m_hWnd=hWnd;
	return Initialize();
}
#endif

bool Input::Initialize()
{
	if(!m_hInst) return false;
	if(m_pKDevice!=0 && m_pMDevice!=0) return true;

	m_bWindowed=g_Config.GetBParameter("Windowed",true);

#ifndef DEEXPORT
	m_hWnd=g_WindowApp.GethWnd();
#endif

	if(!m_hWnd) return false;

	if(!m_pDI)
	{
	//				Creation de Direct Input
		if(DirectInputCreate(m_hInst,
							 DIRECTINPUT_VERSION,
							 &m_pDI,
							 NULL)!=DI_OK)
		{
			g_Output->AddMsg("x Failed creating dinput object\n");
			return false;
		}
	}

//				Creation de la souris

	if(m_pDI->CreateDevice(GUID_SysMouse,&m_pMDevice,NULL)!=DI_OK)
	{
		m_pDI->Release(); m_pDI=0;
		g_Output->AddMsg("x Failed creating mouse device\n");
		return false;
	}

	if(m_pMDevice->SetDataFormat(&c_dfDIMouse)!=DI_OK)
	{
		m_pMDevice->Release(); m_pMDevice=0;
		m_pDI->Release(); m_pDI=0;
		g_Output->AddMsg("x Failed setting mouse format\n");
		return false;
	}

	uint32 dwCoopLevel=DISCL_NONEXCLUSIVE | DISCL_FOREGROUND;

	if(m_pMDevice->SetCooperativeLevel(m_hWnd,dwCoopLevel)!=DI_OK)
	{
		m_pMDevice->Release(); m_pMDevice=0;
		m_pDI->Release(); m_pDI=0;
		g_Output->AddMsg("x Failed setting mouse cooperative level\n");
		return false;
	}

	HANDLE hevent=CreateEvent(0,0,0,0);

	if(m_pMDevice->SetEventNotification(hevent)!=DI_OK)
	{
		m_pMDevice->Release(); m_pMDevice=0;
		m_pDI->Release(); m_pDI=0;
		g_Output->AddMsg("x Failed setting mouse event notification\n");
		return false;
	}

	DIPROPDWORD dipdw=
	{
		{
			sizeof(DIPROPDWORD),
			sizeof(DIPROPHEADER),
			0,
			DIPH_DEVICE,
		},
		32
	};

	if(m_pMDevice->SetProperty(DIPROP_BUFFERSIZE,&dipdw.diph)!=DI_OK)
	{
		m_pMDevice->Release(); m_pMDevice=0;
		m_pDI->Release(); m_pDI=0;
		g_Output->AddMsg("x Failed setting mouse property\n");
		return false;
	}

	m_pMDevice->Acquire();

//					Creation du clavier

	if(m_pDI->CreateDevice(GUID_SysKeyboard,&m_pKDevice,NULL)!=DI_OK)
	{
		m_pMDevice->Release(); m_pMDevice=0;
		m_pDI->Release(); m_pDI=0;
		g_Output->AddMsg("x Failed creating keyboard device\n");
		return false;
	}

	if(m_pKDevice->SetDataFormat(&c_dfDIKeyboard)!=DI_OK)
	{
		m_pKDevice->Release(); m_pKDevice=0;
		m_pMDevice->Release(); m_pMDevice=0;
		m_pDI->Release(); m_pDI=0;
		g_Output->AddMsg("x Failed setting keyboard data format\n");
		return false;
	}

	if(m_pKDevice->SetCooperativeLevel(m_hWnd,dwCoopLevel)!=DI_OK)
	{
		m_pKDevice->Release(); m_pKDevice=0;
		m_pMDevice->Release(); m_pMDevice=0;
		m_pDI->Release(); m_pDI=0;
		g_Output->AddMsg("x Failed setting keyboard cooperative level\n");
		return false;
	}

	m_pKDevice->Acquire();

#ifdef DEEXPORT
	POINT p;
	GetCursorPos(&p);
	ScreenToClient(m_hWnd,&p);

	m_CurrentMState.iX=p.x;
	m_CurrentMState.iY=p.y;
#else
	g_WindowApp.GetCursorPos_(m_CurrentMState.iX,m_CurrentMState.iY);
#endif

	return true;
}

bool Input::Stop()
{
	m_pMDevice->Unacquire();
	m_pKDevice->Unacquire();

	m_pMDevice->Release(); m_pMDevice=0;
	m_pKDevice->Release(); m_pKDevice=0;

	return true;
}

bool Input::OnSuspend()
{
	return true;
}

bool Input::OnResume()
{
	m_pMDevice->Acquire();
	m_pKDevice->Acquire();

#ifdef DEEXPORT
	POINT p;
	GetCursorPos(&p);
	ScreenToClient(m_hWnd,&p);

	m_CurrentMState.iX=p.x;
	m_CurrentMState.iY=p.y;
#else
	g_WindowApp.GetCursorPos_(m_CurrentMState.iX,m_CurrentMState.iY);
#endif

	return true;
}

bool Input::Update()
{
//				Mise a jour de la souris

	DIMOUSESTATE mstate;
	ZeroMemory(&mstate,sizeof(mstate));

	if(m_pMDevice->GetDeviceState(sizeof(mstate),&mstate)==DIERR_INPUTLOST)
	{
		m_pMDevice->Acquire();

		if(m_pMDevice->GetDeviceState(sizeof(mstate),&mstate)==DIERR_INPUTLOST)
			return true;
	}

	m_CurrentMState.iX+=(sint16)mstate.lX;
	m_CurrentMState.iY+=(sint16)mstate.lY;
	m_CurrentMState.iWheel+=(sint16)mstate.lZ;

#ifdef DEEXPORT
	POINT p;
	GetCursorPos(&p);
	ScreenToClient(m_hWnd,&p);

	m_CurrentMState.iX=p.x;
	m_CurrentMState.iY=p.y;
#else
	g_WindowApp.GetCursorPos_(m_CurrentMState.iX,m_CurrentMState.iY);
#endif

	if(m_bClip)
	{
		if(m_CurrentMState.iX < m_iLeft)
			m_CurrentMState.iX=(sint16)m_iLeft;

		if(m_CurrentMState.iX > m_iRight)
			m_CurrentMState.iX=(sint16)m_iRight;

		if(m_CurrentMState.iY < m_iUp)
			m_CurrentMState.iY=m_iUp;

		if(m_CurrentMState.iY > m_iDown)
			m_CurrentMState.iY=m_iDown;
	}

	m_CurrentMState.bLastButtons[0]=m_CurrentMState.bButtons[0];
	m_CurrentMState.bLastButtons[1]=m_CurrentMState.bButtons[1];
	m_CurrentMState.bLastButtons[2]=m_CurrentMState.bButtons[2];

	if(mstate.rgbButtons[0] & 0x80)
		m_CurrentMState.bButtons[0]=true;
	else
		m_CurrentMState.bButtons[0]=false;

	if(mstate.rgbButtons[1] & 0x80)
		m_CurrentMState.bButtons[1]=true;
	else
		m_CurrentMState.bButtons[1]=false;

	if(mstate.rgbButtons[2] & 0x80)
		m_CurrentMState.bButtons[2]=true;
	else
		m_CurrentMState.bButtons[2]=false;
//				Mise a jour du clavier

	memcpy(m_CurrentKState.cLastKeys,m_CurrentKState.cKeys,256);
	m_pKDevice->GetDeviceState(sizeof(m_CurrentKState.cKeys),(void *)m_CurrentKState.cKeys);

	int i;

	for(i=0;i<256;i++)
		if(m_CurrentKState.cKeys[i] & 0x80)
		{
			m_CurrentKState.bKeyDown=true;
			break;
		}

	return true;
}

void Input::GetKeyboardState(KeyboardState& KState)
{
	memcpy(&KState,&m_CurrentKState,sizeof(KeyboardState));
}

void Input::GetMouseState(MouseState& MState)
{
	memcpy(&MState,&m_CurrentMState,sizeof(MouseState));

	if(m_bWindowed)
		MState.iY-=30;
}

char Input::ConvertDIKToASCII(const byte _DIK) const
{
/*	char c;

	switch(_DIK)
	{
		case DIK_A: c='q'; break;
		case DIK_B: c='b'; break;
		case DIK_C:	c='c'; break;
		case DIK_D:	c='d'; break;
		case DIK_E:	c='e'; break;
		case DIK_F: c='f'; break;
		case DIK_G: c='g'; break;
		case DIK_H: c='h'; break;
		case DIK_I: c='i'; break;
		case DIK_J: c='j'; break;
		case DIK_K: c='k'; break;
		case DIK_L: c='l'; break;
		case DIK_M: c=','; break;
		case DIK_N: c='n'; break;
		case DIK_O: c='o'; break;
		case DIK_P: c='p'; break;
		case DIK_Q: c='a'; break;
		case DIK_R: c='r'; break;
		case DIK_S: c='s'; break;
		case DIK_T: c='t'; break;
		case DIK_U: c='u'; break;
		case DIK_V: c='v'; break;
		case DIK_W: c='z'; break;
		case DIK_X: c='x'; break;
		case DIK_Y: c='y'; break;
		case DIK_Z: c='w'; break;
		case DIK_0:
		case DIK_NUMPAD0:
			c='0'; break;
		case DIK_1:
		case DIK_NUMPAD1:
			c='1'; break;
		case DIK_2:
		case DIK_NUMPAD2:
			c='2'; break;
		case DIK_3:
		case DIK_NUMPAD3:
			c='3'; break;
		case DIK_4:
		case DIK_NUMPAD4:
			c='4'; break;
		case DIK_5:
		case DIK_NUMPAD5:
			c='5'; break;
		case DIK_6:
		case DIK_NUMPAD6:
			c='6'; break;
		case DIK_7:
		case DIK_NUMPAD7:
			c='7'; break;
		case DIK_8:
		case DIK_NUMPAD8:
			c='8'; break;
		case DIK_9:
		case DIK_NUMPAD9:
			c='9'; break;
		case DIK_RETURN:
		case DIK_NUMPADENTER:
			c='\n'; break;
		case DIK_SPACE: c=' '; break;
		case DIK_DECIMAL:
		case DIK_COMMA:
			c='.'; break;
		case DIK_PERIOD: c=':'; break;
		case DIK_SLASH: c='!'; break;
		case DIK_BACKSLASH: c='*'; break;
		case DIK_SEMICOLON: c='m'; break;
		case DIK_SUBTRACT: c='-'; break;
		default: c='_';
	}
	return c;*/

	return DIKToASCIITable[_DIK];
}

const char *Input::GetKeyName(const byte _DIK) const
{
	return DIKNameTable[_DIK];
}

void Input::EnableMouseClip(const bool bEnable, int x1, int y1, int x2, int y2)
{
	m_bClip=bEnable;

	m_iLeft=x1; m_iUp=y1;
	m_iRight=x2; m_iDown=y2;

	if(m_bWindowed)
		m_iDown+=30;
}

}
