
#ifndef _IENGINE_H_
#define _IENGINE_H_

#include <windows.h>

class ITextureMgr;
class IShaderMgr;
class IMaterialMgr;
class IMeshMgr;
class ITextOutput;
class IRenderer;
class IFactory;
class IConfig;
class IMap;
class IRenderer2D;

#ifdef DEDEV
class IMemAllocMgr;
class IMemDebugger;
#endif

class IEngine
{
	public:

		IEngine() {}
		virtual ~IEngine() {}

		virtual bool Start(HINSTANCE hInst)=0;
		virtual bool ShutDown()=0;

		virtual ITextureMgr *GetTextureMgr()=0;
		virtual IShaderMgr *GetShaderMgr()=0;
		virtual IMaterialMgr *GetMaterialMgr()=0;
		virtual IMeshMgr *GetMeshMgr()=0;
		virtual ITextOutput *GetTextOutput()=0;
		virtual IRenderer *GetRenderer()=0;
		virtual IFactory *GetFactory()=0;
		virtual IConfig *GetConfig()=0;
		virtual IMap *GetMap()=0;
		virtual IRenderer2D *Get2DRenderer()=0;

#ifdef DEDEV	
		virtual IMemAllocMgr *GetMemAllocMgr()=0;
		virtual IMemDebugger *GetMemDebugger()=0;

#endif

		virtual bool StartMapTest(const char *szSpaceShipName, const char *szPath, DWORD dwThreadID)=0;

		virtual bool UpdateTest()=0;
		virtual bool SuspendTest()=0;
		virtual bool ResumeTest()=0;
		
		virtual bool EndMapTest()=0;

		virtual void EnableLight(bool bEnable)=0;
		virtual void EnableShadows(bool bEnable)=0;

};

#endif
