
#ifndef _ENGINE_H_
#define _ENGINE_H_

#ifdef DEEXPORT

#include "IEngine.h"
#include "../Core/TextureMgr.h"
#include "../Core/ShaderMgr.h"
#include "../Core/MaterialMgr.h"
#include "../Core/MeshMgr.h"
#include "../Core/TextOutput.h"
#include "../Core/Renderer.h"
#include "../Core/Renderer2D.h"
#include "Factory.h"
#include "../Core/Config.h"
#include "../Game/Map.h"

#ifdef DEDEV
#include "../Dev/MemDebugger.h"
#include "../Dev/MemAllocMgr.h"
#endif

namespace Game
{

class SpaceShip;

}

class GameTest;

class Engine : public IEngine
{
	public:

		Engine();
		~Engine();

		bool Start(HINSTANCE hInst);
		bool ShutDown();

		ITextureMgr *GetTextureMgr() { return Core::TextureMgr::GetSingletonPtr(); }
		IShaderMgr *GetShaderMgr() { return Core::ShaderMgr::GetSingletonPtr(); }
		IMaterialMgr *GetMaterialMgr() { return Core::MaterialMgr::GetSingletonPtr(); }
		IMeshMgr *GetMeshMgr() { return Core::MeshMgr::GetSingletonPtr(); }
		ITextOutput *GetTextOutput() { return Core::TextOutput::GetInstance(); }
		IRenderer *GetRenderer() { return Core::Renderer::GetSingletonPtr(); }
		IFactory *GetFactory() { return Factory::GetSingletonPtr(); }
		IConfig *GetConfig() { return Core::Config::GetSingletonPtr(); }
		IMap *GetMap() { return Game::Map::GetSingletonPtr(); }
		IRenderer2D *Get2DRenderer() { return Core::Renderer2D::GetSingletonPtr(); }

#ifdef DEDEV	
		IMemAllocMgr *GetMemAllocMgr() { return Dev::MemAllocMgr::GetSingletonPtr(); }
		IMemDebugger *GetMemDebugger() { return Dev::MemDebugger::GetSingletonPtr(); }
#endif

		bool StartMapTest(const char *szSpaceShipName, const char *szPath,
			DWORD dwThreaID);

		bool UpdateTest();
		bool SuspendTest();
		bool ResumeTest();
		
		bool EndMapTest();

		void EnableLight(bool bEnable);
		void EnableShadows(bool bEnable);

	protected:

		bool StartTest();

		bool m_bTesting;

		bool m_bSuspended;
		uint64 m_dwSuspendTime;
		uint64 m_dwDeltaTime;

		Game::SpaceShip *m_pShip;
		GameTest *m_pTest;

		HWND m_hWnd;
		HINSTANCE m_hInstance;

		bool m_bSavedLight;
		bool m_bSavedShadows;
};

#endif
#endif