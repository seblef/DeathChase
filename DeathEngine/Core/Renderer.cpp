
#include "Renderer.h"
#include "TextureMgr.h"
#include "Renderer2D.h"
#include "ShaderMgr.h"
#include "MaterialMgr.h"
#include "MeshMgr.h"
#include "Config.h"
#include "WindowApp.h"
#include "Kernel.h"
#include "CursorMgr.h"
#include "RenderingCommandBlock.h"
#include "Timer.h"
//#include <dxerr.h>

#include "Texture.h"
#include "Shader.h"
#include "../Exec/Materials/Shaders/ShadersDefs.h"

namespace Core
{

HANDLE g_hRenderingThread=0;
uint32 g_dwRenderingThreadID=0;
bool g_bRenderingFinished=true;

int VertexSizeTable[VX_COUNT]=
{
	sizeof(VertexPNDTT),
	sizeof(VertexPNTT),
	sizeof(VertexPNDT),
	sizeof(VertexPNT),
	sizeof(VertexPDTT),
	sizeof(VertexPTT),
	sizeof(VertexPDT),
	sizeof(VertexPT),
	sizeof(VertexPD),
	sizeof(Vertex2DPDT),
	sizeof(Vertex2DPT),
	sizeof(Vertex2DPD),
	sizeof(VertexP)
};

//				Constante de conversion des couleurs (Byte[0..255] -> Float[0..1])
#define TRANSCOLOR					(0.00392157f)
inline DWORD F2DW( FLOAT f ) { return *((DWORD*)&f); }

// ********************************* Export *********************************************

#ifdef DEEXPORT

Renderer::Renderer() : m_pD3D(0),m_pDevice(0),m_iCurrentView(-1),
	m_LastVertDecl(VX_COUNT),m_hWnd(0),m_bMultiThreadingEnable(false),
	m_hInstance(0), m_iMultiSample(0),m_iAnisotropic(0),m_iShadersVersion(14),
	m_iCurrentExecBuffer(1), m_iCurrentFillBuffer(0),
	m_iCurrentBlock(-1), m_dwRenderingTime(0)
{
	int i;
	for(i=0;i<VX_COUNT;i++)
		m_pVertDecl[i]=0;

	RenderingCommandBlock::InitAllocator();
	memset(m_pCurrentBlock,0,MAX_COMMANDBLOCK_DEPTH*sizeof(RenderingCommandBlock*));

	m_ExecTable[RC_NOEXEC]=&Renderer::ExecNone;
	m_ExecTable[RC_EXECCOMMANDBLOCK]=&Renderer::ExecNone;
	m_ExecTable[RC_SETSTATEBLOCK]=&Renderer::ExecStateBlock;
	m_ExecTable[RC_SETRENDERSTATE]=&Renderer::ExecRenderState;
	m_ExecTable[RC_SETTEXTURE]=&Renderer::ExecSetTexture;
	m_ExecTable[RC_SETSHADER]=&Renderer::ExecSetShader;
	m_ExecTable[RC_SETTRANSFORMATION]=&Renderer::ExecSetTransformation;
	m_ExecTable[RC_SETVERTEXSHADERCONSTANT]=&Renderer::ExecSetVertexShaderConstant;
	m_ExecTable[RC_SETVERTEXDECLARATION]=&Renderer::ExecSetVertexDeclaration;
	m_ExecTable[RC_CLEARBUFFER]=&Renderer::ExecClearBuffer;
	m_ExecTable[RC_SETVERTEXBUFFER]=&Renderer::ExecSetVertexBuffer;
	m_ExecTable[RC_SETINDEXBUFFER]=&Renderer::ExecSetIndexBuffer;
	m_ExecTable[RC_RENDERSTATICBUFFERS]=&Renderer::ExecRenderStaticBuffers;
	m_ExecTable[RC_RENDERDYNAMICBUFFERS]=&Renderer::ExecRenderDynamicBuffers;
	m_ExecTable[RC_SETMATERIAL]=&Renderer::ExecSetMaterial;
}

Renderer::~Renderer()
{}

bool Renderer::Initialize(HWND hWnd, HINSTANCE hInst, int iWidth, int iHeight)
{
	if(m_pD3D)
		return true;

	if((m_pD3D=Direct3DCreate9(D3D_SDK_VERSION))==NULL)
	{
		g_Output->AddMsg("x Erreur lors de la création de l'objet d3d9\n");
		return false;
	}

	if(m_pDevice)
		return true;

	m_hInstance=hInst;
	if(!m_hInstance) return false;

	m_hWnd=hWnd;
	if(!m_hWnd) return false;

	m_iAnisotropic=g_Config.GetIParameter("Anisotropic",0);

	const char *szShader=g_Config.GetSParameter("ShadersDetails","low");
	m_iShadersVersion=14;
	if(strcmp(szShader,"medium")==0) m_iShadersVersion=20;
	else if(strcmp(szShader,"high")==0) m_iShadersVersion=30;

	D3DPRESENT_PARAMETERS pp;
	ZeroMemory(&pp,sizeof(pp));
	pp.Windowed=true;
	pp.hDeviceWindow=m_hWnd;

	//			Destermination du type de ZBuffer

	pp.EnableAutoDepthStencil=TRUE;
	pp.AutoDepthStencilFormat=D3DFMT_D24S8;

	D3DDISPLAYMODE d3d_dm;
	m_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT,&d3d_dm);
	pp.BackBufferWidth=10;//d3d_dm.Width;
	pp.BackBufferHeight=10;//d3d_dm.Height;
	pp.BackBufferFormat=d3d_dm.Format;
	pp.SwapEffect=D3DSWAPEFFECT_COPY;
	pp.PresentationInterval=D3DPRESENT_INTERVAL_IMMEDIATE;

	if(m_pD3D->CreateDevice(D3DADAPTER_DEFAULT,
						 D3DDEVTYPE_HAL,
						 m_hWnd,
						 D3DCREATE_HARDWARE_VERTEXPROCESSING,
						 &pp,
						 &m_pDevice)!=D3D_OK)
	{
		if(m_pD3D->CreateDevice(D3DADAPTER_DEFAULT,
							 D3DDEVTYPE_HAL,
							 m_hWnd,
							 D3DCREATE_SOFTWARE_VERTEXPROCESSING,
							 &pp,
							 &m_pDevice)!=D3D_OK)
		{
			m_pD3D->Release();
			m_pD3D=0;
			g_Output->AddMsg("x Aucune carte trouvée\n");
			return false;
		}
		else
			g_Output->AddMsg("! Non-TnL device found\n");
	}
	else
		g_Output->AddMsg("! TnL device found\n");

	memcpy(&m_DeviceParams,&pp,sizeof(D3DPRESENT_PARAMETERS));

	AddView(hWnd,iWidth,iHeight);
	m_iCurrentView=0;

	InitRenderState();
	BuildVertexDeclarations();

	new MaterialMgr(m_pDevice);
	new ShaderMgr(m_pDevice);
	new TextureMgr(m_pDevice);
	new MeshMgr(m_pDevice);
	new ParticleEngine;
	g_ParticleEngine.Load();
	
	D3DADAPTER_IDENTIFIER9 id;
	m_pD3D->GetAdapterIdentifier(D3DADAPTER_DEFAULT,0,&id);

	g_Output->AddMsg("! <----------------- Video device ------------------>\n\n");
	g_Output->AddMsg("    Name: %s\n    Description: %s\n    Driver: %s,%i\n",
		id.DeviceName,id.Description,id.Driver,id.DriverVersion);
	g_Output->AddMsg("    Texture memory: %i\n\n",m_pDevice->GetAvailableTextureMem());
	g_Output->AddMsg("! <------------------------------------------------->\n");

	BuildVertexDeclarations();
	new Renderer2D(m_pDevice);

	const float fTime[4]={0.0f,0.0f,0.0f,0.0f};

	SetVertexShaderConstant(COMMON_TIME_REG,fTime);

	return true;
}

void Renderer::BeginScene()
{
	if(m_iCurrentView!=-1 && m_pDevice)
	{
		if(m_bMultiThreadingEnable && !g_bRenderingFinished)
			WaitForSingleObject(g_hRenderingThread,INFINITE);

		HRESULT res=m_pDevice->TestCooperativeLevel();

		if(res==D3DERR_DEVICELOST)
		{
			g_Output->AddMsg("x Video device lost\n");
			return;
		}
		else if(res==D3DERR_DEVICENOTRESET)
			if(!ResetDevice())
			{
				g_Output->AddMsg("x Failed reseting video device\n");
				return;
			}

		m_pDevice->BeginScene();

		if(m_bMultiThreadingEnable)
		{
			NewFrame();
			ExecuteThread();
		}

		ClearBuffer(D3DCLEAR_ZBUFFER | D3DCLEAR_TARGET,0);
	}
}

void Renderer::EndScene(bool bForceExecute)
{
	if(m_iCurrentView!=-1 && m_pDevice && !m_bMultiThreadingEnable)
	{
		NewFrame();
		Execute();
	}
}

bool Renderer::ResetDevice()
{
	D3DPRESENT_PARAMETERS pp;
	memcpy(&pp,&m_DeviceParams,sizeof(D3DPRESENT_PARAMETERS));

	if(m_pDevice->Reset(&pp)!=D3D_OK)
		return false;

	InitRenderState();

	return true;
}

void Renderer::Shutdown()
{
	g_Renderer2D.ShutDown();

	int i;
	for(i=0;i<VX_COUNT;i++)
	{
		if(m_pVertDecl[i]) m_pVertDecl[i]->Release();
		m_pVertDecl[i]=0;
	}

	if(m_Views.size() > 1)
		for(i=1;i<m_Views.size();i++)
		{
			if(m_Views[i].pZBuffer) m_Views[i].pZBuffer->Release();
			if(m_Views[i].pBackBuffer) m_Views[i].pBackBuffer->Release();
			if(m_Views[i].pSwapChain) m_Views[i].pSwapChain->Release();
		}

	m_Views.clear();

	m_pDevice->Release(); m_pDevice=0;
	m_pD3D->Release(); m_pD3D=0;
}

void Renderer::SetViewport(const Vector2i &vViewportPos, const Vector2i& vViewportSize)
{
	MYASSERT(m_pDevice,"Renderer device not created",true);

	if(m_iCurrentView==-1)
		return;

	if(vViewportPos.x >= 0 && vViewportPos.y >= 0 &&
			vViewportSize.x > 0 && vViewportSize.y > 0 &&
			(vViewportPos.x + vViewportSize.x) <= m_Views[m_iCurrentView].vSize.x &&
			(vViewportPos.y + vViewportSize.y) <= m_Views[m_iCurrentView].vSize.y)
	{
		MYASSERT(0,"Bad viewport coords",true);
		return;
	}

	D3DVIEWPORT9 dview;
	dview.X=vViewportPos.x; dview.Y=vViewportPos.y;
	dview.Width=vViewportSize.x; dview.Height=vViewportSize.y;
	dview.MinZ=0.0f; dview.MaxZ=1.0f;

	ASSERTRETURN(m_pDevice->SetViewport(&dview),D3D_OK,"Setting D3D Viewport");

	m_Views[m_iCurrentView].vViewportPos=vViewportPos;
	m_Views[m_iCurrentView].vViewportSize=vViewportSize;
}

int Renderer::GetView(HWND hWnd)
{
	int counter=0;
	std::vector<View>::iterator i;
	for(i=m_Views.begin();i!=m_Views.end();i++,counter++)
		if(i->hWnd==hWnd)
			return counter;

	return -1;
}

bool Renderer::AddView(HWND hWnd, int iWidth, int iHeight)
{
	if(GetView(hWnd)!=-1 || !m_pDevice || !hWnd)
		return false;

	if(m_bMultiThreadingEnable && !g_bRenderingFinished)
		WaitForSingleObject(g_hRenderingThread,INFINITE);

	View v(hWnd,Vector2i(iWidth,iHeight),Vector2i::NullVector,Vector2i(iWidth,iHeight));
	D3DPRESENT_PARAMETERS pp;
	memset(&pp,0,sizeof(pp));
	pp.BackBufferWidth=iWidth;
	pp.BackBufferHeight=iHeight;
	pp.BackBufferFormat=m_DeviceParams.BackBufferFormat;
	pp.BackBufferCount=1;
	pp.EnableAutoDepthStencil=false;
	pp.MultiSampleType=D3DMULTISAMPLE_NONE;
	pp.SwapEffect=D3DSWAPEFFECT_COPY;
	pp.Windowed=true;
	pp.hDeviceWindow=hWnd;
	pp.PresentationInterval=D3DPRESENT_INTERVAL_IMMEDIATE;

	if(m_pDevice->CreateAdditionalSwapChain(&pp,&v.pSwapChain)!=D3D_OK)
		return false;

	if(m_pDevice->CreateDepthStencilSurface((UINT)iWidth,(UINT)iHeight,
		D3DFMT_D24S8,D3DMULTISAMPLE_NONE,0,FALSE,&v.pZBuffer,0)!=D3D_OK)
	{
		v.pSwapChain->Release();
		return false;
	}

	v.pSwapChain->GetBackBuffer(0,D3DBACKBUFFER_TYPE_MONO,&v.pBackBuffer);

	m_Views.push_back(v);
	m_iCurrentView=-1;
	return true;
}

bool Renderer::RemoveView(HWND hWnd)
{
	const int iView=GetView(hWnd);
	if(iView==-1)
		return false;

	if(m_bMultiThreadingEnable && !g_bRenderingFinished)
		WaitForSingleObject(g_hRenderingThread,INFINITE);

	if(m_Views[iView].pZBuffer) m_Views[iView].pZBuffer->Release();
	if(m_Views[iView].pBackBuffer) m_Views[iView].pBackBuffer->Release();
	if(m_Views[iView].pSwapChain) m_Views[iView].pSwapChain->Release();

	m_Views.erase(m_Views.begin()+iView);
	m_iCurrentView=-1;
	return true;
}

bool Renderer::SetView(HWND hWnd)
{
	const int iView=GetView(hWnd);
	if(iView==-1 || !m_pDevice) return false;
	if(iView==m_iCurrentView) return true;

	if(m_bMultiThreadingEnable && !g_bRenderingFinished)
		WaitForSingleObject(g_hRenderingThread,INFINITE);

	if(m_pDevice->SetDepthStencilSurface(m_Views[iView].pZBuffer)!=D3D_OK)
		return false;

	if(m_pDevice->SetRenderTarget(0,m_Views[iView].pBackBuffer)!=D3D_OK)
		return false;

	m_iCurrentView=iView;
	return true;
}

HWND Renderer::GetCurrentView() const
{
	if(m_iCurrentView==-1)
		return 0;
	else
		return m_Views[m_iCurrentView].hWnd;
}

bool Renderer::ResizeView(HWND hWnd, int iWidth, int iHeight)
{
	if(m_bMultiThreadingEnable && !g_bRenderingFinished)
		WaitForSingleObject(g_hRenderingThread,INFINITE);

	const int iView=GetView(hWnd);
	if(iView==-1) return false;
	if(iWidth==m_Views[iView].vSize.x && iHeight==m_Views[iView].vSize.y) return true;

	const bool bCurrent= iView==m_iCurrentView;

	if(!RemoveView(hWnd)) return false;
	if(!AddView(hWnd,iWidth,iHeight)) return false;

	return true;
}

bool Renderer::DrawImageInView(HTexture hTex, HWND hWnd)
{
	const int iView=GetView(hWnd);
	if(iView==-1) return false;
	if(iView!=m_iCurrentView && !SetView(hWnd))	return false;

	BeginScene();

	g_Renderer2D.Start();
	g_Renderer2D.Draw2DImage(hTex,m_Views[m_iCurrentView].vViewportPos.x,m_Views[m_iCurrentView].vViewportPos.y
		,m_Views[m_iCurrentView].vViewportSize.x,m_Views[m_iCurrentView].vViewportSize.y);
	g_Renderer2D.End();

	EndScene();

	return true;
}

const Vector2i& Renderer::GetScreenSize() const
{
	if(m_iCurrentView!=-1)
		return m_Views[m_iCurrentView].vSize;
	else
		return Vector2i::NullVector;
}

const Vector2i& Renderer::GetViewportPos() const
{
	if(m_iCurrentView!=-1)
		return m_Views[m_iCurrentView].vViewportPos;
	else
		return Vector2i::NullVector;
}

const Vector2i& Renderer::GetViewportSize() const
{
	if(m_iCurrentView!=-1)
		return m_Views[m_iCurrentView].vViewportSize;
	else
		return Vector2i::NullVector;
}

#else

// ****************************** In-Game *************************************************

Renderer::Renderer(const int iPriority, HINSTANCE hInst) : ITask(iPriority),
	m_vScreenSize(0,0),m_vViewportPos(0,0),m_vViewportSize(0,0),
	m_pD3D(0),m_pDevice(0),
	m_bWindowed(false), m_LastVertDecl(VX_COUNT),
	m_hInstance(hInst), m_iMultiSample(0),m_iAnisotropic(0),m_iShadersVersion(14),
	m_iCurrentExecBuffer(1), m_iCurrentFillBuffer(0),
	m_iCurrentBlock(-1), m_dwRenderingTime(0),
	m_bClearColorBuffer(false)
{
	strcpy(m_szName,"Renderer");

	int i;
	for(i=0;i<VX_COUNT;i++)
		m_pVertDecl[i]=0;

	RenderingCommandBlock::InitAllocator();
	memset(m_pCurrentBlock,0,MAX_COMMANDBLOCK_DEPTH*sizeof(RenderingCommandBlock*));

	m_ExecTable[RC_NOEXEC]=&Renderer::ExecNone;
	m_ExecTable[RC_EXECCOMMANDBLOCK]=&Renderer::ExecNone;
	m_ExecTable[RC_SETSTATEBLOCK]=&Renderer::ExecStateBlock;
	m_ExecTable[RC_SETRENDERSTATE]=&Renderer::ExecRenderState;
	m_ExecTable[RC_SETTEXTURE]=&Renderer::ExecSetTexture;
	m_ExecTable[RC_SETSHADER]=&Renderer::ExecSetShader;
	m_ExecTable[RC_SETTRANSFORMATION]=&Renderer::ExecSetTransformation;
	m_ExecTable[RC_SETVERTEXSHADERCONSTANT]=&Renderer::ExecSetVertexShaderConstant;
	m_ExecTable[RC_SETVERTEXDECLARATION]=&Renderer::ExecSetVertexDeclaration;
	m_ExecTable[RC_CLEARBUFFER]=&Renderer::ExecClearBuffer;
	m_ExecTable[RC_SETVERTEXBUFFER]=&Renderer::ExecSetVertexBuffer;
	m_ExecTable[RC_SETINDEXBUFFER]=&Renderer::ExecSetIndexBuffer;
	m_ExecTable[RC_RENDERSTATICBUFFERS]=&Renderer::ExecRenderStaticBuffers;
	m_ExecTable[RC_RENDERDYNAMICBUFFERS]=&Renderer::ExecRenderDynamicBuffers;
	m_ExecTable[RC_SETMATERIAL]=&Renderer::ExecSetMaterial;
}

Renderer::~Renderer()
{}

bool Renderer::Initialize()
{
	if(m_pD3D)
		return true;

	if((m_pD3D=Direct3DCreate9(D3D_SDK_VERSION))==NULL)
	{
		g_Output->AddMsg("x Erreur lors de la création de l'objet d3d9\n");
		return false;
	}

	if(!CreateDevice())
		return false;
	
	new ShaderMgr(m_pDevice);
	new TextureMgr(m_pDevice);
	new MaterialMgr(m_pDevice);
	new MeshMgr(m_pDevice);
	
	D3DADAPTER_IDENTIFIER9 id;
	m_pD3D->GetAdapterIdentifier(D3DADAPTER_DEFAULT,0,&id);

	g_Output->AddMsg("! <----------------- Video device ------------------>\n\n");
	g_Output->AddMsg("    Name: %s\n    Description: %s\n    Driver: %s,%i\n",
		id.DeviceName,id.Description,id.Driver,id.DriverVersion);
	g_Output->AddMsg("    Texture memory: %i\n\n",m_pDevice->GetAvailableTextureMem());
	g_Output->AddMsg("! <------------------------------------------------->\n");

	BuildVertexDeclarations();
	new Renderer2D(m_pDevice);
	new CursorMgr(m_pDevice);
	g_CursorMgr.Enable(true);

	return true;
}

bool Renderer::Update()
{
	BeginScene();
	return true;
}

bool Renderer::OnSuspend()
{
//	ClearRenderingBuffers();
	return true;
}

bool Renderer::OnResume()
{
	HRESULT res=m_pDevice->TestCooperativeLevel();

	if(res==D3DERR_DEVICENOTRESET)
		if(!ResetDevice())
		{
			g_Output->AddMsg("x Failed reseting device\n");
			return false;
		}

	return true;
}

bool Renderer::Stop()
{
	ClearRenderingBuffers();
	return true;
}

bool Renderer::BeginScene()
{
	if(!g_bRenderingFinished)
		WaitForSingleObject(g_hRenderingThread,INFINITE);

	HRESULT res=m_pDevice->TestCooperativeLevel();

	if(res==D3DERR_DEVICELOST)
	{
		g_Output->AddMsg("x Video device lost\n");
		return false;
	}
	else if(res==D3DERR_DEVICENOTRESET)
		if(!ResetDevice())
		{
			g_Output->AddMsg("x Failed reseting video device\n");
			return false;
		}

	m_pDevice->BeginScene();

	NewFrame();
	ExecuteThread();

	static uint32 dwLastTime=timeGetTime();
	uint32 dwNow=timeGetTime();

	const float fThisTime=((float)(dwNow - dwLastTime)) * 0.001f;
	const float fTime[4]={fThisTime,fThisTime,fThisTime,fThisTime};

	dwLastTime=dwNow;

	if(m_bClearColorBuffer)
		ClearBuffer(D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,0);
	else
		ClearBuffer(D3DCLEAR_ZBUFFER,0);
	
	//			Demarrage du rendu
//	SetVertexShaderConstant(COMMON_TIME_REG,fTime);
	return true;
}

bool Renderer::EndScene()
{
	m_pDevice->EndScene();
	return true;
}

bool Renderer::ResetDevice()
{
	D3DPRESENT_PARAMETERS pp;
	memcpy(&pp,&m_DeviceParams,sizeof(D3DPRESENT_PARAMETERS));

	if(m_pDevice->Reset(&pp)!=D3D_OK)
		return false;

	InitRenderState();

	return true;
}

void Renderer::Shutdown()
{
	g_CursorMgr.Release();
	g_Renderer2D.ShutDown();

	int i;
	for(i=0;i<VX_COUNT;i++)
	{
		if(m_pVertDecl[i]) m_pVertDecl[i]->Release();
		m_pVertDecl[i]=0;
	}

	m_pDevice->Release(); m_pDevice=0;
	m_pD3D->Release(); m_pD3D=0;
}

void Renderer::SetViewport(const Vector2i &vViewportPos, const Vector2i& vViewportSize)
{
	MYASSERT(m_pDevice,"Renderer device not created",true);

	if(vViewportPos.x >= 0 && vViewportPos.y >= 0 &&
			vViewportSize.x > 0 && vViewportSize.y > 0 &&
			(vViewportPos.x + vViewportSize.x) <= m_vScreenSize.x &&
			(vViewportPos.y + vViewportSize.y) <= m_vScreenSize.y)
	{
		MYASSERT(0,"Bad viewport coords",true);
		return;
	}

	D3DVIEWPORT9 dview;
	dview.X=vViewportPos.x; dview.Y=vViewportPos.y;
	dview.Width=vViewportSize.x; dview.Height=vViewportSize.y;
	dview.MinZ=0.0f; dview.MaxZ=1.0f;

	ASSERTRETURN(m_pDevice->SetViewport(&dview),D3D_OK,"Setting D3D Viewport");

	m_vViewportPos=vViewportPos;
	m_vViewportSize=vViewportSize;
}

bool Renderer::CreateDevice()
{
	if(m_pDevice)
		return true;

	if(!m_hInstance) return false;

	m_hWnd=g_WindowApp.GethWnd();
	if(!m_hWnd) return false;

	int iWidth,iHeight,iFullWidth,iFullHeight;
	bool bWindowed;
	uint16 wWidth,wHeight;
	
	iWidth=g_Config.GetIParameter("WindowWidth",640);
	iHeight=g_Config.GetIParameter("WindowHeight",480);
	bWindowed=g_Config.GetBParameter("Windowed",true);
	iFullWidth=g_Config.GetIParameter("FullscreenWidth",640);
	iFullHeight=g_Config.GetIParameter("FullscreenHeight",480);
	m_iMultiSample=g_Config.GetIParameter("MultiSample",0);
	m_iAnisotropic=g_Config.GetIParameter("Anisotropic",0);

	const char *szShader=g_Config.GetSParameter("ShadersDetails","low");
	m_iShadersVersion=14;
	if(strcmp(szShader,"medium")==0) m_iShadersVersion=20;
	else if(strcmp(szShader,"high")==0) m_iShadersVersion=30;

	wWidth=(uint16)(bWindowed ? iWidth : iFullWidth);
	wHeight=(uint16)(bWindowed ? iHeight : iFullHeight);

	D3DPRESENT_PARAMETERS pp;
	ZeroMemory(&pp,sizeof(pp));
	pp.Windowed=bWindowed;
	pp.BackBufferCount=1;
	pp.hDeviceWindow=m_hWnd;

	//			Destermination du type de ZBuffer

	pp.EnableAutoDepthStencil=TRUE;

	if(bWindowed)
	{
		pp.BackBufferWidth=0;
		pp.BackBufferHeight=0;

		D3DDISPLAYMODE d3d_dm;
		m_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT,&d3d_dm);
		pp.BackBufferFormat=d3d_dm.Format;
		pp.SwapEffect=D3DSWAPEFFECT_COPY;
		pp.PresentationInterval=D3DPRESENT_INTERVAL_IMMEDIATE;
		pp.AutoDepthStencilFormat=D3DFMT_D24S8;
	}
	else
	{
		pp.BackBufferWidth=wWidth;
		pp.BackBufferHeight=wHeight;
		pp.BackBufferCount=2;
		pp.PresentationInterval=D3DPRESENT_INTERVAL_IMMEDIATE;
		pp.BackBufferFormat=D3DFMT_X8R8G8B8;
		pp.SwapEffect=D3DSWAPEFFECT_FLIP;
		pp.AutoDepthStencilFormat=D3DFMT_D24S8;
	}

	if(m_iMultiSample!=0)
	{
		D3DMULTISAMPLE_TYPE ms=D3DMULTISAMPLE_NONE;
		if(m_iMultiSample==2) ms=D3DMULTISAMPLE_2_SAMPLES;
		else if(m_iMultiSample==4) ms=D3DMULTISAMPLE_4_SAMPLES;
		else if(m_iMultiSample==8) ms=D3DMULTISAMPLE_8_SAMPLES;

		DWORD quality;

		if(ms!=D3DMULTISAMPLE_NONE)
		{
			m_pD3D->CheckDeviceMultiSampleType(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,pp.BackBufferFormat,
				bWindowed ? TRUE : FALSE, ms,&quality);

			pp.MultiSampleType=ms;
			pp.MultiSampleQuality=quality;
		}
	}

	if(bWindowed)
	{
		if(m_pD3D->CreateDevice(D3DADAPTER_DEFAULT,
							 D3DDEVTYPE_HAL,
							 m_hWnd,
							 D3DCREATE_HARDWARE_VERTEXPROCESSING,
							 &pp,
							 &m_pDevice)!=D3D_OK)
		{
			if(m_pD3D->CreateDevice(D3DADAPTER_DEFAULT,
								 D3DDEVTYPE_HAL,
								 m_hWnd,
								 D3DCREATE_SOFTWARE_VERTEXPROCESSING,
								 &pp,
								 &m_pDevice)!=D3D_OK)
			{
				m_pD3D->Release();
				m_pD3D=0;
				g_Output->AddMsg("x Aucune carte trouvée\n");
				return false;
			}
			else
				g_Output->AddMsg("! Non-TnL device found\n");
		}
		else
			g_Output->AddMsg("! TnL device found\n");
	}
	else
	{
		if(m_pD3D->CreateDevice(D3DADAPTER_DEFAULT,
							 D3DDEVTYPE_HAL,
							 m_hWnd,
							 D3DCREATE_HARDWARE_VERTEXPROCESSING,
							 &pp,
							 &m_pDevice)!=D3D_OK)
		{
			if(m_pD3D->CreateDevice(D3DADAPTER_DEFAULT,
								 D3DDEVTYPE_HAL,
								 m_hWnd,
								 D3DCREATE_SOFTWARE_VERTEXPROCESSING,
								 &pp,
								 &m_pDevice)!=D3D_OK)
			{
				m_pD3D->Release();
				m_pD3D=0;
				g_Output->AddMsg("x Aucune carte trouvée\n");
				return false;
			}
			else
				g_Output->AddMsg("! Non-TnL device found\n");
		}
		else
			g_Output->AddMsg("!  TnL device found\n");
	}

	m_vScreenSize=m_vViewportSize=Vector2i(wWidth,wHeight);
	m_vViewportPos=Vector2i(0,0);
	m_bWindowed=bWindowed;

	memcpy(&m_DeviceParams,&pp,sizeof(D3DPRESENT_PARAMETERS));

	InitRenderState();

	BuildVertexDeclarations();

	return true;
}

bool Renderer::Restart()
{
	int iWidth,iHeight,iFullWidth,iFullHeight;
	bool bWindowed;
	int iMultiSample;
	int iShadersVersion;
	
	iWidth=g_Config.GetIParameter("WindowWidth",640);
	iHeight=g_Config.GetIParameter("WindowHeight",480);
	bWindowed=g_Config.GetBParameter("Windowed",true);
	iFullWidth=g_Config.GetIParameter("FullscreenWidth",640);
	iFullHeight=g_Config.GetIParameter("FullscreenHeight",480);
	iMultiSample=g_Config.GetIParameter("MultiSample",0);

	const char *szShader=g_Config.GetSParameter("ShadersDetails","low");
	iShadersVersion=14;
	if(strcmp(szShader,"medium")==0) iShadersVersion=20;
	else if(strcmp(szShader,"high")==0) iShadersVersion=30;

	bool bShadersChanged=m_iShadersVersion!=iShadersVersion;
	bool bChanged=(iWidth!=m_vScreenSize.x) || (iHeight!=m_vScreenSize.y) ||
		(iMultiSample!=m_iMultiSample) || (bWindowed!=m_bWindowed);
	
	if(bShadersChanged)
		g_ShaderMgr.BeginRestart();

	if(bChanged)
	{
		g_Renderer2D.BeginRestart();

		int i;
		for(i=0;i<VX_COUNT;i++)
		{
			if(m_pVertDecl[i]) m_pVertDecl[i]->Release();
			m_pVertDecl[i]=0;
		}

		g_TextureMgr.BeginRestart();
		if(!bShadersChanged) g_ShaderMgr.BeginRestart();

		m_pDevice->Release();
		m_pDevice=0;
		if(!CreateDevice())
		{
			g_Kernel.KillAllTasks();
			return false;
		}

		if(!bShadersChanged) g_ShaderMgr.EndRestart(m_pDevice);
		g_TextureMgr.EndRestart(m_pDevice);

		g_Renderer2D.EndRestart(m_pDevice);
	}

	if(bShadersChanged)
		g_ShaderMgr.EndRestart(m_pDevice);

	return true;
}
#endif

// *********************************** Commun **********************************************


IDirect3DVertexBuffer9 *Renderer::CreateVertexBuffer(void *pVertices,
													 uint32 dwCount,
													 VertexType _Type)
{
	MYASSERT(_Type >= 0 && _Type < VX_COUNT,"Bad vertex format",true);
	MYASSERT(dwCount > 0,"Vertices count must be > 0",true);

	IDirect3DVertexBuffer9 *pVBuf;
	if(m_pDevice->CreateVertexBuffer(dwCount*VertexSizeTable[_Type],
		0,0,D3DPOOL_MANAGED,&pVBuf,0)!=D3D_OK)
	{
		g_Output->AddMsg("x Failed creating vertex buffer of %i vertices\n",dwCount);
		return 0;
	}

	if(pVertices)
	{
		void *pBuffer;
		if(pVBuf->Lock(0,dwCount*VertexSizeTable[_Type],(void **)&pBuffer,0)!=D3D_OK)
		{
			g_Output->AddMsg("x Failed locking vertex buffer\n");
			pVBuf->Release();
			return 0;
		}

		memcpy(pBuffer,pVertices,dwCount*VertexSizeTable[_Type]);
		ASSERTRETURN(pVBuf->Unlock(),D3D_OK,"Unlocking vertex buffer");
	}

	return pVBuf;
}

IDirect3DIndexBuffer9 *Renderer::CreateIndexBuffer(void *pIndices,
												   uint32 dwCount,
												   D3DFORMAT _Format)
{
	MYASSERT(dwCount > 0,"Indices count me best > 0",true);
	
	IDirect3DIndexBuffer9 *pIBuf;
	int iSize;
	if(_Format==D3DFMT_INDEX16) iSize=sizeof(uint16); else iSize=sizeof(uint32);
	if(m_pDevice->CreateIndexBuffer(dwCount*iSize,0,_Format,D3DPOOL_MANAGED,&pIBuf,0)!=D3D_OK)
	{
		g_Output->AddMsg("x Failed creating index buffer of %i indices\n",dwCount);
		return 0;
	}

	if(pIndices)
	{
		void *pBuffer;
		if(pIBuf->Lock(0,dwCount*iSize,(void **)&pBuffer,0)!=D3D_OK)
		{
			pIBuf->Release();
			g_Output->AddMsg("x Failed locking index buffer\n");
			return 0;
		}

		memcpy(pBuffer,pIndices,dwCount*iSize);
		ASSERTRETURN(pIBuf->Unlock(),D3D_OK,"Unlocking index buffer");
	}

	return pIBuf;
}


void Renderer::BuildVertexDeclarations()
{
	D3DVERTEXELEMENT9 vePNDTT[]={
		{ 0, 0, D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0,12, D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
		{ 0,24, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
		{ 0,28, D3DDECLTYPE_FLOAT2,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		{ 0,36, D3DDECLTYPE_FLOAT2,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1 },
		D3DDECL_END() };
	D3DVERTEXELEMENT9 vePNTT[]={
		{ 0, 0, D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0,12, D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
		{ 0,24, D3DDECLTYPE_FLOAT2,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		{ 0,32, D3DDECLTYPE_FLOAT2,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1 },
		D3DDECL_END() };
	D3DVERTEXELEMENT9 vePNDT[]={
		{ 0, 0, D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0,12, D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
		{ 0,24, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
		{ 0,28, D3DDECLTYPE_FLOAT2,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		D3DDECL_END() };
	D3DVERTEXELEMENT9 vePNT[]={
		{ 0, 0, D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0,12, D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
		{ 0,24, D3DDECLTYPE_FLOAT2,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		D3DDECL_END() };
	D3DVERTEXELEMENT9 vePDTT[]={
		{ 0, 0, D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0,12, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
		{ 0,16, D3DDECLTYPE_FLOAT2,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		{ 0,24, D3DDECLTYPE_FLOAT2,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1 },
		D3DDECL_END() };
	D3DVERTEXELEMENT9 vePTT[]={
		{ 0, 0, D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0,12, D3DDECLTYPE_FLOAT2,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		{ 0,20, D3DDECLTYPE_FLOAT2,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1 },
		D3DDECL_END() };
	D3DVERTEXELEMENT9 vePDT[]={
		{ 0, 0, D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0,12, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
		{ 0,16, D3DDECLTYPE_FLOAT2,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		D3DDECL_END() };
	D3DVERTEXELEMENT9 vePT[]={
		{ 0, 0, D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0,12, D3DDECLTYPE_FLOAT2,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		D3DDECL_END() };
	D3DVERTEXELEMENT9 vePD[]={
		{ 0, 0, D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0,12, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
		D3DDECL_END() };
	D3DVERTEXELEMENT9 ve2DPDT[]={
		{ 0, 0, D3DDECLTYPE_FLOAT4,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITIONT, 0 },
		{ 0,16, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
		{ 0,20, D3DDECLTYPE_FLOAT2,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		D3DDECL_END() };
	D3DVERTEXELEMENT9 ve2DPT[]={
		{ 0, 0, D3DDECLTYPE_FLOAT4,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITIONT, 0 },		{ 0,16, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
		{ 0,16, D3DDECLTYPE_FLOAT2,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		D3DDECL_END() };
	D3DVERTEXELEMENT9 ve2DPD[]={
		{ 0, 0, D3DDECLTYPE_FLOAT4,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITIONT, 0 },
		{ 0,16, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
		D3DDECL_END() };
	D3DVERTEXELEMENT9 veP[]={
		{ 0, 0, D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		D3DDECL_END() };

	D3DVERTEXELEMENT9 *pve[VX_COUNT]={ vePNDTT, vePNTT, vePNDT, vePNT, vePDTT, vePTT, vePDT,
		vePT, vePD, ve2DPDT, ve2DPT, ve2DPD, veP };

	int i;
	for(i=0;i<VX_COUNT;i++)
	{
		ASSERTRETURN(m_pDevice->CreateVertexDeclaration(pve[i],&m_pVertDecl[i]),D3D_OK,"");
	}
}

void Renderer::InitRenderState()
{
	const float bias=-1.0f;

	m_pDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);
	m_pDevice->SetRenderState(D3DRS_FOGVERTEXMODE,D3DFOG_LINEAR);
	m_pDevice->SetRenderState(D3DRS_LIGHTING,FALSE);
	m_pDevice->SetRenderState(D3DRS_ZFUNC,D3DCMP_LESSEQUAL);
	m_pDevice->SetSamplerState(0,D3DSAMP_MAGFILTER,D3DTEXF_LINEAR);
	m_pDevice->SetSamplerState(0,D3DSAMP_MINFILTER,D3DTEXF_LINEAR);
	m_pDevice->SetSamplerState(0,D3DSAMP_MIPFILTER,D3DTEXF_LINEAR);
	m_pDevice->SetSamplerState(1,D3DSAMP_MAGFILTER,D3DTEXF_LINEAR);
	m_pDevice->SetSamplerState(1,D3DSAMP_MINFILTER,D3DTEXF_LINEAR);
	m_pDevice->SetSamplerState(1,D3DSAMP_MIPFILTER,D3DTEXF_LINEAR);
	m_pDevice->SetSamplerState(2,D3DSAMP_MAGFILTER,D3DTEXF_LINEAR);
	m_pDevice->SetSamplerState(2,D3DSAMP_MINFILTER,D3DTEXF_LINEAR);
	m_pDevice->SetSamplerState(2,D3DSAMP_MIPFILTER,D3DTEXF_LINEAR);
	m_pDevice->SetSamplerState(3,D3DSAMP_MAGFILTER,D3DTEXF_LINEAR);
	m_pDevice->SetSamplerState(3,D3DSAMP_MINFILTER,D3DTEXF_LINEAR);
	m_pDevice->SetSamplerState(3,D3DSAMP_MIPFILTER,D3DTEXF_LINEAR);
	m_pDevice->SetSamplerState(0,D3DSAMP_MIPMAPLODBIAS,*((LPDWORD)(&bias)));
	m_pDevice->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_DIFFUSE);
	m_pDevice->SetTextureStageState(0,D3DTSS_COLORARG2,D3DTA_TEXTURE);
	m_pDevice->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_DIFFUSE);
	m_pDevice->SetTextureStageState(0,D3DTSS_ALPHAARG2,D3DTA_TEXTURE);
	m_pDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCCOLOR);
	m_pDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCCOLOR);
	m_pDevice->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE,D3DMCS_MATERIAL);
	m_pDevice->SetRenderState(D3DRS_AMBIENT,MYRGB(40,40,40));

	D3DMATERIAL9 mat;
	mat.Ambient.r=mat.Diffuse.r=1.0f;
	mat.Ambient.g=mat.Diffuse.g=1.0f;
	mat.Ambient.b=mat.Diffuse.b=1.0f;
	
	m_pDevice->SetMaterial(&mat);
}


// ********************************* Rendering Queue ***************************************


TMemoryAllocator<RenderingCommandBlock> *RenderingCommandBlock::m_spAllocator=0;

void RenderingCommandBlock::InitAllocator()
{
	if(!m_spAllocator)
		m_spAllocator=new TMemoryAllocator<RenderingCommandBlock>("RenderingCommandBlock");
}

RenderingCommandBlock *RenderingCommandBlock::GetNew()
{
	return m_spAllocator->New();
}

void RenderingCommandBlock::Delete(RenderingCommandBlock *pBlock)
{
	m_spAllocator->Delete(pBlock);
}

#ifdef DEEXPORT
void Renderer::EnableMultiThreading(bool bEnable)
{
	ClearRenderingBuffers();
	m_bMultiThreadingEnable=bEnable;
}
#endif

void Renderer::ClearRenderingBuffers()
{
	if(!g_bRenderingFinished)
		WaitForSingleObject(g_hRenderingThread,INFINITE);

	m_Buffers[0].clear();
	m_Buffers[1].clear();
	m_VertBuffers[0].Clear();
	m_VertBuffers[1].Clear();
	m_IndBuffers[0].Clear();
	m_IndBuffers[1].Clear();
	m_TransParamBuffers[0].Clear();
	m_TransParamBuffers[1].Clear();
	m_ShaderConstantParamBuffers[0].Clear();
	m_ShaderConstantParamBuffers[1].Clear();
	m_RenderDynBuffersParamBuffers[0].Clear();
	m_RenderDynBuffersParamBuffers[1].Clear();
}

bool Renderer::NewFrame()
{
	if(m_iCurrentExecBuffer==1)
	{
		m_iCurrentExecBuffer=0;
		m_iCurrentFillBuffer=1;
	}
	else
	{
		m_iCurrentExecBuffer=1;
		m_iCurrentFillBuffer=0;
	}

#ifdef DEDEV
	m_strErrorBuffer.clear();
#endif

	return true;
}

DWORD WINAPI RenderingThread(LPVOID pParam)
{
	CRITICAL_SECTION Sync;

	InitializeCriticalSection(&Sync);
	EnterCriticalSection(&Sync);

	g_Renderer.Execute();

	LeaveCriticalSection(&Sync);
	DeleteCriticalSection(&Sync);

	g_bRenderingFinished=true;

	return 0;
}

bool Renderer::ExecuteThread()
{
	g_bRenderingFinished=false;
	g_hRenderingThread=CreateThread(NULL,NULL,RenderingThread,NULL,NULL,&g_dwRenderingThreadID);
	return true;
}

bool Renderer::Execute()
{
#ifdef DEEXPORT
	const int iView=m_iCurrentView;
#endif

	uint64 dwStartTime=g_Timer.GetTimeInMicroS();

	std::vector<RenderingOp>::iterator i;
	for(i=m_Buffers[m_iCurrentExecBuffer].begin();i!=m_Buffers[m_iCurrentExecBuffer].end();i++)
		ExecuteOp(*i);

	m_Buffers[m_iCurrentExecBuffer].clear();
	m_VertBuffers[m_iCurrentExecBuffer].Clear();
	m_IndBuffers[m_iCurrentExecBuffer].Clear();
	m_TransParamBuffers[m_iCurrentExecBuffer].Clear();
	m_ShaderConstantParamBuffers[m_iCurrentExecBuffer].Clear();
	m_RenderDynBuffersParamBuffers[m_iCurrentExecBuffer].Clear();

#ifdef DEEXPORT
	m_pDevice->EndScene();
#else
	EndScene();
#endif

#ifdef DEDEV
		if(HasExecErrors())
			g_Output->AddMsg(GetErrorBuffer().c_str());
#endif

#ifdef DEEXPORT
	if(m_Views[iView].pSwapChain)
		m_Views[iView].pSwapChain->Present(NULL,NULL,NULL,NULL,0);
#else
	m_pDevice->Present(0,0,0,0);
#endif

	m_dwRenderingTime=g_Timer.GetTimeInMicroS() - dwStartTime;

	return true;
}

bool Renderer::StartCommandBlock()
{
	m_iCurrentBlock++;
	if(m_iCurrentBlock >= MAX_COMMANDBLOCK_DEPTH)
	{
		m_iCurrentBlock=MAX_COMMANDBLOCK_DEPTH-1;
		return false;
	}
	else
	{
		if(m_pCurrentBlock[m_iCurrentBlock])
			m_pCurrentBlock[m_iCurrentBlock]->Clear();
		else
			m_pCurrentBlock[m_iCurrentBlock]=RenderingCommandBlock::GetNew();
	}

	return true;
}

IRenderingCommandBlock *Renderer::EndCommandBlock()
{
	if(m_iCurrentBlock==-1) return 0;
	
	IRenderingCommandBlock *pBlock=m_pCurrentBlock[m_iCurrentBlock];
	m_pCurrentBlock[m_iCurrentBlock]=0;
	m_iCurrentBlock--;

	return pBlock;
}

void Renderer::ReleaseCommandBlock(IRenderingCommandBlock *pBlock)
{
	if(pBlock)
		RenderingCommandBlock::Delete((RenderingCommandBlock *)pBlock);
}

bool Renderer::AddCommandBlock(IRenderingCommandBlock *pBlock)
{
	return AddOp(RenderingOp(RC_EXECCOMMANDBLOCK,(void *)pBlock));
}

bool Renderer::AddOp(const RenderingOp &op)
{
	MYASSERT(op.IsValid(),"Commande de rendu invalide",true);

	if(m_iCurrentBlock==-1 || m_pCurrentBlock[m_iCurrentBlock]==0)
		m_Buffers[m_iCurrentFillBuffer].push_back(op);
	else
		m_pCurrentBlock[m_iCurrentBlock]->AddOp(op);

	return true;
}

bool Renderer::ExecuteOp(const RenderingOp &op)
{
	MYASSERT(op.iCmd >= RC_NOEXEC && op.iCmd < RC_COUNT,"Commande de rendu inconnue",true);

#ifdef DEDEV
	bool bRet;

	if(op.iCmd==RC_EXECCOMMANDBLOCK)
	{
		if(op.pParam)
		{
			bRet=true;

			const std::vector<RenderingOp> *pOps=((RenderingCommandBlock *)op.pParam)->GetOps();
			std::vector<RenderingOp>::const_iterator i;

			for(i=pOps->begin();i!=pOps->end();i++)
				bRet&=ExecuteOp(*i);
		}
		else
			bRet=false;
	}
	else
		bRet=(this->*m_ExecTable[op.iCmd])(op);

	if(!bRet)
		AddErrorMsg("x  RenderingExecError: [%s], %x, %i, %i\n",szRenderingOpNames[op.iCmd],op.pParam,op.dwParam1,op.dwParam2);

	return bRet;
#else
	if(op.iCmd==RC_EXECCOMMANDBLOCK)
	{
		if(op.pParam)
		{
			bool bRet=true;

			const std::vector<RenderingOp> *pOps=((RenderingCommandBlock *)op.pParam)->GetOps();
			std::vector<RenderingOp>::const_iterator i;

			for(i=pOps->begin();i!=pOps->end();i++)
				bRet&=ExecuteOp(*i);

			return bRet;
		}
		else
			return false;
	}
	else
		return (this->*m_ExecTable[op.iCmd])(op);
#endif
}

#ifdef DEDEV
void Renderer::AddErrorMsg(const char *szMsg,...)
{
	va_list args;
	va_start(args,szMsg);

	static char szTempBuffer[1024];
	vsprintf(szTempBuffer,szMsg,args);

	m_strErrorBuffer+=szTempBuffer;
}
#endif

bool Renderer::SetTransformation(TransformationType type,
									   const Matrix4& mTrans,
									   const Matrix4 *pInverse,
									   bool bInverse)
{
	TransParam pParam;
	pParam.mTrans=mTrans;
	
	if(pInverse)
	{
		pParam.mInverse=*pInverse;
		pParam.bSetInverse=true;
		pParam.bBuildInverse=false;
	}
	else if(bInverse)
	{
		pParam.bBuildInverse=true;
		pParam.bSetInverse=true;
	}
	else
		pParam.bSetInverse=false;

	pParam._Type=type;
	
	const int iIndex=m_TransParamBuffers[m_iCurrentFillBuffer].GetItemCount();
	m_TransParamBuffers[m_iCurrentFillBuffer].AddByMemcpy(&pParam);

	return AddOp(RenderingOp(RC_SETTRANSFORMATION,0,(uint32)type,iIndex));
}

bool Renderer::SetVertexShaderConstant(uint32 dwReg, const float *fConstant,int iCount)
{
	if(iCount==0 || iCount > 16 || !fConstant) return false;

	ShaderConstantParam pParam;;
	pParam.iReg=dwReg;
	pParam.iCount=iCount;
	memcpy(pParam.fConstant,fConstant,iCount*4*sizeof(float));

	const int iIndex=m_ShaderConstantParamBuffers[m_iCurrentFillBuffer].GetItemCount();
	m_ShaderConstantParamBuffers[m_iCurrentFillBuffer].AddByMemcpy(&pParam);

	return AddOp(RenderingOp(RC_SETVERTEXSHADERCONSTANT,0,iIndex));
}

bool Renderer::RenderDynamicBuffers(D3DPRIMITIVETYPE type,
										  int iVertCount, void *pVertBuffer, VertexType VertType,
										  int iIndCount, void *pInd, D3DFORMAT IndFormat)
{
	if(!pVertBuffer || iVertCount==0) return false;

	RenderDynBuffersParam pParam;
	pParam.dwIndCount=iIndCount;
	pParam.dwIndOffset=m_IndBuffers[m_iCurrentFillBuffer].GetItemCount();
	pParam.dwVertCount=iVertCount;
	pParam.dwVertOffset=m_VertBuffers[m_iCurrentFillBuffer].GetItemCount();
	pParam.VertType=VertType;
	pParam.IndFormat=IndFormat;
	pParam.PrimitiveType=type;

	if(iIndCount > 0)
		m_IndBuffers[m_iCurrentFillBuffer].AddTab((byte*)pInd,iIndCount * (IndFormat==D3DFMT_INDEX32 ? sizeof(uint32) : sizeof(uint16)));
	
	m_VertBuffers[m_iCurrentFillBuffer].AddTab((byte*)pVertBuffer,iVertCount * VertexSizeTable[VertType]);

	const int iIndex=m_RenderDynBuffersParamBuffers[m_iCurrentFillBuffer].GetItemCount();
	m_RenderDynBuffersParamBuffers[m_iCurrentFillBuffer].AddByMemcpy(&pParam);

	return AddOp(RenderingOp(RC_RENDERDYNAMICBUFFERS,0,iIndex));
}

bool Renderer::ExecNone(const RenderingOp& op)
{
	return false;
}

bool Renderer::ExecStateBlock(const RenderingOp& op)
{
#ifdef DEDEV
	if(op.pParam)
	{
		HRESULT hRes=((IDirect3DStateBlock9 *)op.pParam)->Apply();
		if(hRes==D3D_OK)
			return true;
		else
		{
//			AddErrorMsg("x  IDirect3DStateBlock9::Apply(): %s, %s\n",
//				DXGetErrorString(hRes),DXGetErrorDescription(hRes));
			return false;
		}
	}
	else
		return false;
#else
	if(op.pParam)
		return ((IDirect3DStateBlock9 *)op.pParam)->Apply()==D3D_OK;
	else
		return false;
#endif
}

bool Renderer::ExecRenderState(const RenderingOp& op)
{
#ifdef DEDEV
	HRESULT hRes=m_pDevice->SetRenderState((D3DRENDERSTATETYPE)op.dwParam1,op.dwParam2);
	if(hRes==D3D_OK)
		return true;
	else
	{
//		AddErrorMsg("x  IDirect3DDevice9::SetRenderState(): %s, %s\n",
//			DXGetErrorString(hRes),DXGetErrorDescription(hRes));
		return false;
	}
#else
	return m_pDevice->SetRenderState((D3DRENDERSTATETYPE)op.dwParam1,op.dwParam2)==D3D_OK;
#endif
}

bool Renderer::ExecSetTexture(const RenderingOp& op)
{
	Texture *pTex=(Texture *)((IResource *)op.pParam);

#ifdef DEDEV
	if(pTex)
	{
		HRESULT hRes=m_pDevice->SetTexture(op.dwParam1,pTex->GetTexture());
		if(hRes==D3D_OK)
			return true;
		else
		{
//			AddErrorMsg("x  IDirect3DDevice9::SetTexture(): %s, %s\n",
//				DXGetErrorString(hRes),DXGetErrorDescription(hRes));
			return false;
		}
	}
	else
	{
		m_pDevice->SetTexture(op.dwParam1,0);
		return false;
	}
#else
	if(pTex && pTex->GetTexture())
		return m_pDevice->SetTexture(op.dwParam1,pTex->GetTexture())==D3D_OK;
	else
		return false;
#endif
}

bool Renderer::ExecSetShader(const RenderingOp& op)
{
	Shader *pShader=(Shader *)((IResource *)op.pParam);

#ifdef DEDEV
	if(pShader && pShader->GetPixelShader() && pShader->GetVertexShader())
	{
		HRESULT hRes=m_pDevice->SetVertexShader(pShader->GetVertexShader());
		if(hRes!=D3D_OK)
		{
//			AddErrorMsg("x  IDirect3DDevice9::SetVertexShader(): %s, %s\n",
//				DXGetErrorString(hRes),DXGetErrorDescription(hRes));
			return false;
		}

		hRes=m_pDevice->SetPixelShader(pShader->GetPixelShader());
		if(hRes==D3D_OK)
			return true;
		else
		{
			//AddErrorMsg("x  IDirect3DDevice9::SetPixelShader(): %s, %s\n",
			//	DXGetErrorString(hRes),DXGetErrorDescription(hRes));
			return false;
		}
	}
	else
		return false;
#else
	if(pShader && pShader->GetPixelShader() && pShader->GetVertexShader())
	{
		return (m_pDevice->SetVertexShader(pShader->GetVertexShader())==D3D_OK) &&
			(m_pDevice->SetPixelShader(pShader->GetPixelShader())==D3D_OK);
	}
	else
		return false;
#endif
}

bool Renderer::ExecSetTransformation(const RenderingOp& op)
{
	TransParam *pParam=m_TransParamBuffers[m_iCurrentExecBuffer].GetBuffer() + op.dwParam2;
	if(pParam)
	{
		Matrix4 mTransposed;
		pParam->mTrans.Transpose(mTransposed);

		if(pParam->_Type==TRANS_VIEWPROJ)
			m_pDevice->SetVertexShaderConstantF(MAT_VIEWPROJ_REG,(const float *)mTransposed.m,4);
		else
		{
			m_pDevice->SetVertexShaderConstantF(MAT_WORLD_REG,(const float *)mTransposed.m,4);
			mTransposed.m[0][3]=0.0f;
			mTransposed.m[1][3]=0.0f;
			mTransposed.m[2][3]=0.0f;
			m_pDevice->SetVertexShaderConstantF(MAT_ROTWORLD_REG,(const float *)mTransposed.m,4);

			if(pParam->bSetInverse)
			{
				if(pParam->bBuildInverse)
					pParam->mTrans.Inverse(pParam->mInverse);

				pParam->mInverse.Transpose(mTransposed);
//				m_pDevice->SetVertexShaderConstantF(MAT_INVWORLDT_REG,(const float *)pParam->mInverse.m,4);
				m_pDevice->SetVertexShaderConstantF(MAT_INVWORLD_REG,(const float *)mTransposed.m,4);
			}
		}

		return true;
	}
	else
		return false;
}

bool Renderer::ExecSetVertexShaderConstant(const RenderingOp& op)
{
	ShaderConstantParam *pParam=m_ShaderConstantParamBuffers[m_iCurrentExecBuffer].GetBuffer() + op.dwParam1;
	if(pParam)
	{
		HRESULT hRes=m_pDevice->SetVertexShaderConstantF(pParam->iReg,pParam->fConstant,pParam->iCount)==D3D_OK;

#ifdef DEDEV
		if(hRes==D3D_OK)
			return true;
		else
		{
//			AddErrorMsg("x  IDirect3DDevice9::SetVertexShaderConstantF(): %s, %s\n",
//				DXGetErrorString(hRes),DXGetErrorDescription(hRes));
			return true;
		}
#else
		return hRes==D3D_OK;
#endif
	}
	else
		return false;
}

bool Renderer::ExecSetVertexDeclaration(const RenderingOp& op)
{
	if(m_LastVertDecl==op.dwParam1)
		return true;
#ifdef DEDEV
	HRESULT hRes=m_pDevice->SetVertexDeclaration(m_pVertDecl[op.dwParam1]);
	if(hRes==D3D_OK)
	{
		m_LastVertDecl=(Core::VertexType)op.dwParam1;
		return true;
	}
	else
	{
//		AddErrorMsg("x  IDirect3DDevice9::SetVertexDeclaration(): %s, %s\n",
//			DXGetErrorString(hRes),DXGetErrorDescription(hRes));
		return false;
	}
#else
	if(m_pDevice->SetVertexDeclaration(m_pVertDecl[op.dwParam1])==D3D_OK)
	{
		m_LastVertDecl=(VertexType)op.dwParam1;
		return true;
	}
	else
		return false;
#endif
}

bool Renderer::ExecClearBuffer(const RenderingOp &op)
{
#ifdef DEEXPORT
#define CLEAR_COLOR				0x00888888
#else
#define CLEAR_COLOR				0x00000000
#endif

#ifdef DEDEV
	HRESULT hRes=m_pDevice->Clear(0,0,op.dwParam1,CLEAR_COLOR,1.0f,op.dwParam2);
	if(hRes==D3D_OK)
		return true;
	else
	{
//		AddErrorMsg("x  IDirect3DDevice9::Clear(): %s, %s\n",
//			DXGetErrorString(hRes),DXGetErrorDescription(hRes));
		return false;
	}
#else
	return m_pDevice->Clear(0,0,op.dwParam1,CLEAR_COLOR,1.0f,op.dwParam2)==D3D_OK;
#endif
}

bool Renderer::ExecSetVertexBuffer(const RenderingOp& op)
{
	m_dwCurrentVertCount=op.dwParam1;

#ifdef DEDEV
	HRESULT hRes=m_pDevice->SetStreamSource(0,(IDirect3DVertexBuffer9 *)op.pParam,0,op.dwParam2);
	if(hRes==D3D_OK)
		return true;
	else
	{
//		AddErrorMsg("x  IDirect3DDevice9::SetVertexBuffer(): %s, %s\n",
//			DXGetErrorString(hRes),DXGetErrorDescription(hRes));
		return false;
	}
#else
	return m_pDevice->SetStreamSource(0,(IDirect3DVertexBuffer9 *)op.pParam,0,op.dwParam2)==D3D_OK;
#endif
}

bool Renderer::ExecSetIndexBuffer(const RenderingOp &op)
{
#ifdef DEDEV
	HRESULT hRes=m_pDevice->SetIndices((IDirect3DIndexBuffer9 *)op.pParam);
	if(hRes==D3D_OK)
		return true;
	else
	{
//		AddErrorMsg("x  IDirect3DDevice9::SetIndices(): %s, %s\n",
//			DXGetErrorString(hRes),DXGetErrorDescription(hRes));
		return false;
	}
#else
	return m_pDevice->SetIndices((IDirect3DIndexBuffer9 *)op.pParam)==D3D_OK;
#endif
}

bool Renderer::ExecRenderStaticBuffers(const RenderingOp &op)
{
#ifdef DEDEV
	HRESULT hRes=m_pDevice->DrawIndexedPrimitive((const D3DPRIMITIVETYPE)((const uint32)op.pParam),0,0,m_dwCurrentVertCount,
		op.dwParam1,op.dwParam2);
	if(hRes==D3D_OK)
		return true;
	else
	{
//		AddErrorMsg("x  IDirect3DDevice9::DrawIndexedPrimitive(): %s, %s\n",
//			DXGetErrorString(hRes),DXGetErrorDescription(hRes));
		return false;
	}
#else
	return m_pDevice->DrawIndexedPrimitive((const D3DPRIMITIVETYPE)((const uint32)op.pParam),0,0,m_dwCurrentVertCount,
		op.dwParam1,op.dwParam2)==D3D_OK;
#endif
}

bool Renderer::ExecRenderDynamicBuffers(const RenderingOp &op)
{
	RenderDynBuffersParam *pParam=m_RenderDynBuffersParamBuffers[m_iCurrentExecBuffer].GetBuffer() + op.dwParam1;
	if(pParam)
	{
		uint32 dwPrimCount;
		HRESULT hRes;

		if(pParam->dwIndCount==0)
		{
			switch(pParam->PrimitiveType)
			{
				case D3DPT_POINTLIST:
					dwPrimCount=pParam->dwVertCount;
					break;

				case D3DPT_LINELIST:
					dwPrimCount=pParam->dwVertCount / 2;
					break;

				case D3DPT_LINESTRIP:
					dwPrimCount=pParam->dwVertCount-1;
					break;

				case D3DPT_TRIANGLELIST:
					dwPrimCount=pParam->dwVertCount / 3;
					break;

				case D3DPT_TRIANGLESTRIP:
					dwPrimCount=pParam->dwVertCount - 2;
					break;

				case D3DPT_TRIANGLEFAN:
					dwPrimCount=pParam->dwVertCount - 1;
					break;

				default:
					return false;
			}

			hRes=m_pDevice->DrawPrimitiveUP(pParam->PrimitiveType,dwPrimCount,
				(void *)(m_VertBuffers[m_iCurrentExecBuffer].GetBuffer() + pParam->dwVertOffset),
				VertexSizeTable[pParam->VertType]);
		}
		else
		{
			switch(pParam->PrimitiveType)
			{
				case D3DPT_POINTLIST:
					dwPrimCount=pParam->dwIndCount;
					break;

				case D3DPT_LINELIST:
					dwPrimCount=pParam->dwIndCount / 2;
					break;

				case D3DPT_LINESTRIP:
					dwPrimCount=pParam->dwIndCount-1;
					break;

				case D3DPT_TRIANGLELIST:
					dwPrimCount=pParam->dwIndCount / 3;
					break;

				case D3DPT_TRIANGLESTRIP:
					dwPrimCount=pParam->dwIndCount - 2;
					break;

				case D3DPT_TRIANGLEFAN:
					dwPrimCount=pParam->dwIndCount - 1;
					break;

				default:
					return false;
			}

			hRes=m_pDevice->DrawIndexedPrimitiveUP(pParam->PrimitiveType,0,
				pParam->dwVertCount,dwPrimCount,
				(void *)(m_IndBuffers[m_iCurrentExecBuffer].GetBuffer() + pParam->dwIndOffset),
				pParam->IndFormat,
				(void *)(m_VertBuffers[m_iCurrentExecBuffer].GetBuffer() + pParam->dwVertOffset),
				VertexSizeTable[pParam->VertType]);
		}

#ifdef DEDEV
		if(hRes==D3D_OK)
			return true;
		else
		{
//			AddErrorMsg("x  IDirect3DDevice9::DrawIndexedPrimitiveUP(): %s, %s\n",
//				DXGetErrorString(hRes),DXGetErrorDescription(hRes));
			return false;
		}
#else
		return hRes==D3D_OK;
#endif
	}
	else
		return false;
}

bool Renderer::ExecSetMaterial(const RenderingOp& op)
{
#ifdef DEDEV
	HRESULT hRes=m_pDevice->SetMaterial((D3DMATERIAL9 *)op.pParam);
	if(hRes==D3D_OK)
		return true;
	else
	{
//		AddErrorMsg("x  IDirect3DDevice9::SetMaterial(): %s, %s\n",
//			DXGetErrorString(hRes),DXGetErrorDescription(hRes));
		return false;
	}
#else
	return m_pDevice->SetMaterial((D3DMATERIAL9 *)op.pParam);
#endif
}


}