
#ifndef _INTERFACES_H_
#define _INTERFACES_H_

#include <windows.h>
#include "../DeathEngine/Core/TSingleton.h"
#include "../DeathEngine/Export/IEngine.h"

#ifdef DEDEV
#include "../DeathEngine/Export/IMemDebugger.h"
#include "../DeathEngine/Export/IMemAllocMgr.h"
#endif

class Interfaces : public Core::TSingleton<Interfaces>
{
	public:

		Interfaces() : m_pEngine(0),m_hEngine(0) {}
		~Interfaces() { Release(); }

		bool Load(HINSTANCE hAppInst);
		void Release();

		IEngine *GetEngine() const { return m_pEngine; }
		IConfig *GetConfig() const { return m_pEngine->GetConfig(); }
		IFactory *GetFactory() const { return m_pEngine->GetFactory(); }
		IRenderer *GetRenderer() const { return m_pEngine->GetRenderer(); }
		IShaderMgr *GetShaderMgr() const { return m_pEngine->GetShaderMgr(); }
		ITextOutput *GetTextOutput() const { return m_pEngine->GetTextOutput(); }
		ITextureMgr *GetTextureMgr() const { return m_pEngine->GetTextureMgr(); }
		IMeshMgr *GetMeshMgr() const { return m_pEngine->GetMeshMgr(); }
		IMaterialMgr *GetMaterialMgr() const { return m_pEngine->GetMaterialMgr(); }
		IMap *GetMap() const { return m_pEngine->GetMap(); }
		IRenderer2D *Get2DRenderer() const { return m_pEngine->Get2DRenderer(); }

	private:

		IEngine *m_pEngine;
		HINSTANCE m_hEngine;
};

#define g_Interfaces			Interfaces::GetSingletonRef()
#define g_Engine				g_Interfaces.GetEngine()
#define g_Config				g_Interfaces.GetConfig()
#define g_Factory				g_Interfaces.GetFactory()
#define g_Renderer				g_Interfaces.GetRenderer()
#define g_2DRenderer			g_Interfaces.Get2DRenderer()
#define g_ShaderMgr				g_Interfaces.GetShaderMgr()
#define g_TextureMgr			g_Interfaces.GetTextureMgr()
#define g_MeshMgr				g_Interfaces.GetMeshMgr()
#define g_MaterialMgr			g_Interfaces.GetMaterialMgr()
#define g_Map					g_Interfaces.GetMap()
#define g_Output2				g_Interfaces.GetTextOutput()

#endif