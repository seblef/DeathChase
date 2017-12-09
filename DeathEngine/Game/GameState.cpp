
#include "GameState.h"
#include "Map.h"
#include "SpaceShip.h"
#include "Octree.h"
#include "RenderingQueue.h"
#include "CollisionMgr.h"
#include "ExplosionFactory.h"
#include "../GUI/Font.h"
#include "../Core/Renderer.h"
#include "../Core/Renderer2D.h"
#include "../Core/MeshMgr.h"
#include "../Core/Timer.h"
#include "../Core/Input.h"
#include "../Core/ParticleEngine.h"
#include "../Core/Sound.h"
#include "../Core/SoundVirtualSource.h"


namespace Game
{

GameState::GameState(const int iPriority) : Core::ITask(iPriority), m_pShip(0),
	m_dwDeltaTime(0),m_dwFrameCount(0),m_pMusicSource(0)
{
	strcpy(m_szName,"GameState");
	new Map;
	new CollisionMgr;
}

GameState::~GameState()
{
}

bool GameState::Initialize()
{
	return true;
}

bool GameState::Stop()
{
	if(m_pMusicSource)
	{
		g_Sound.StopSound(m_pMusicSource);
		g_Sound.ReleaseSound(m_hMusic);
		delete m_pMusicSource;
	}

	g_Map.Release();
	if(m_pShip) delete m_pShip;
	m_pShip=0;

	if(m_pFont) delete m_pFont;
	m_pFont=0;

	return true;
}

bool GameState::OnSuspend()
{
	m_dwSuspendTime=g_Timer.GetTimeInMicroS();

	return true;
}

bool GameState::OnResume()
{
	m_dwDeltaTime+=g_Timer.GetTimeInMicroS() - m_dwSuspendTime;

	return true;
}

bool GameState::Update()
{
	uint64 dwTime=g_Timer.GetTimeInMicroS() - m_dwDeltaTime;

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

	Core::Input::KeyboardState kState;
	g_Input.GetKeyboardState(kState);

	m_Control.ProcessKeys((char *)kState.cKeys,(char *)kState.cLastKeys);

	g_CollisionMgr.ProcessMessages();

	g_Map.Simulate(fTime);
	g_ParticleEngine.Update(fTime);

	g_Octree.Collision();
	g_CollisionMgr.ProcessCollisions();

	g_Renderer.SetTransformation(TRANS_VIEWPROJ,m_Camera.GetViewProjMatrix());
	g_Renderer.SetTransformation(TRANS_MODEL,Core::Matrix4::Identity,0,true);

	const float fTimeCt[4]={fTotalTime,fTotalTime,fTotalTime,fTotalTime};

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

	m_dwFrameCount++;

	return true;
}

bool GameState::Load(const char *szMapFileName,
					 const char *szMapPath,
					 const char *szSpaceShipName,
					 const char *szSpaceShipPath)
{
	g_Output->AddMsg("\n! -- Loading particles systems --\n");
	g_ParticleEngine.Load();
	g_Output->AddMsg("! -- Loading explosions --\n\n");
	g_ExplosionFactory.Load();

	std::string strMapName(szMapFileName);
	strMapName=strMapName.substr(0,strMapName.length()-5);

	g_Output->AddMsg("\n! -- Loading map %s%s --\n\n",szMapPath,strMapName.c_str());

	if(!g_Map.Load(strMapName.c_str(),szMapPath))
		return false;

	g_Output->AddMsg("\n! -- Loading spaceship %s%s --\n\n",szSpaceShipPath, szSpaceShipName);

	if(m_pShip)
		m_pShip->Clear();
	else
		m_pShip=new SpaceShip;

	if(!m_pShip->Load(szSpaceShipName,szSpaceShipPath))
	{
		g_Map.Release();
		delete m_pShip;
		m_pShip=0;
		return false;
	}

	float fMidMap=((float)g_Map.GetHeightmap()->GetSize())*0.5f;
	float fHeight=g_Map.GetHeightmap()->GetHeight((int)fMidMap,(int)fMidMap);
	m_pShip->GetTransformation().SetPosition(Core::Vector3(fMidMap,fHeight+10.0f,fMidMap));

	Core::Vector2i iViewSize(g_Renderer.GetViewportSize());

	m_Camera.Initialize(CAM_PERSPECTIVE,0.5f,5000.0f,(float)iViewSize.x*0.5f,
		(float)iViewSize.y*0.5f,90,1.0f);
	m_Camera.ActiveFollowShip();

	m_pShip->SetCamera(&m_Camera);
	m_pShip->SetController(&m_Control);
	g_Octree.InsertObject(m_pShip);

	g_RenderingQueue.SetCamera(&m_Camera);

	m_pFont=new GUI::Font(0,"lucidasmall.bmp");
	((GUI::Font *)m_pFont)->Load("lucidasmall.bmp");

	g_Renderer.EnableClearingColorBuffer(false);

	m_hMusic=g_Sound.LoadSound("Tool.ogg",0);
	m_pMusicSource=g_Sound.Play(m_hMusic,0,false,true);
	m_pMusicSource->SetGain(1.0f);

	return true;
}

}

