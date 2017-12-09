
#ifdef DEEXPORT

#include "GameTest.h"

#include "Core/Renderer.h"
#include "Core/Renderer2D.h"
#include "Core/Input.h"
#include "Core/MeshMgr.h"

#include "GUI/Environment.h"
#include "GUI/Font.h"

#include "Game/Map.h"
#include "Game/Octree.h"
#include "Game/RenderingQueue.h"


GameTest::GameTest(Game::SpaceShip *pShip, HWND hWnd) : m_pShip(pShip),m_hWnd(hWnd),
	m_pFont(0)
{
	RECT rect;
	GetClientRect(hWnd,&rect);

	m_Camera.Initialize(CAM_PERSPECTIVE,0.5f,5000.0f,(float)(rect.right-rect.left)*0.5f,
		(float)(rect.bottom-rect.top)*0.5f,90,1.0f);
	m_Camera.ActiveFollowShip();

	m_pShip->SetCamera(&m_Camera);
	m_pShip->SetController(&m_PlayerControl);

	g_Octree.InsertObject(m_pShip);

	g_Renderer.AddView(m_hWnd,rect.right-rect.left,rect.bottom-rect.top);

	m_pFont=new GUI::Font(0,"lucidasmall.bmp");
	((GUI::Font *)m_pFont)->Load("lucidasmall.bmp");
}

void GameTest::Clear()
{
	g_Renderer.RemoveView(m_hWnd);
	if(m_pFont) delete m_pFont;
	m_pFont=0;
}

bool GameTest::Initialize()
{
	return g_Input.Initialize(m_hWnd);
}

bool GameTest::Stop()
{
	m_pShip->RemoveFromOctree();

	return g_Input.Stop();
}

bool GameTest::Update(const uint64 dwTime)
{
	static uint64 dwLastTime;
	static bool bFirst=true;
	static float fTotalTime=0.0f;

	float fFPS;
	float fTime;

	if(bFirst)
	{
		fFPS=0.0f;
		fTime=0.0f;
		bFirst=false;
	}
	else
	{
		fTime=((float)(dwTime - dwLastTime)) * 1e-6f;
		fFPS=KINV(fTime);
	}

	dwLastTime=dwTime;
	fTotalTime+=fTime;

	float fRenderingTime=(float)g_Renderer.GetRenderingTime() * 1e-6;
	float fPercRendering=fRenderingTime *fFPS;

	const float fTimeCt[4]={fTotalTime,fTotalTime,fTotalTime,fTotalTime};

	g_Input.Update();
	Core::Input::KeyboardState kState;
	g_Input.GetKeyboardState(kState);

	m_PlayerControl.ProcessKeys((char *)kState.cKeys,(char *)kState.cLastKeys);
	g_Map.Simulate(fTime);

	g_Renderer.SetView(m_hWnd);
	g_Renderer.BeginScene();

	g_Renderer.SetTransformation(TRANS_VIEWPROJ,m_Camera.GetViewProjMatrix());
	g_Renderer.SetTransformation(TRANS_MODEL,Core::Matrix4::Identity,0,true);

	g_Renderer.SetVertexShaderConstant(COMMON_TIME_REG,fTimeCt);

	g_Map.Visibility(m_Camera.GetFrustrum());
	g_RenderingQueue.Execute(g_Map.GetSun()->GetDirection());

	if(m_pFont)
	{
		char szBuf[128];
		sprintf(szBuf,"FPS: %f",fFPS);

		g_Renderer2D.Start();
		m_pFont->Draw(szBuf,Core::Rect(10,10,200,50),Core::Color32(255,200,200,200));
		sprintf(szBuf,"Rendering%: %6.1f",fPercRendering * 100.0f);
		m_pFont->Draw(szBuf,Core::Rect(10,50,200,100),Core::Color32(255,200,200,200));
		g_Renderer2D.Execute();
	}

	g_Renderer.EndScene(false);

	return true;
}


#endif