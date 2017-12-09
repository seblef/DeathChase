
#ifndef _RENDERER_H_

#define _RENDERER_H_

#include "ITask.h"
#include "Rect.h"
#include "Handles.h"
#include "TSingleton.h"
#include "TDynamicTab.h"
#include "TMemoryAllocator.h"
#include "IResource.h"

#ifdef DEEXPORT
#include "../Export/IRenderer.h"
#include <vector>
#else

#include "../Exec/Materials/Shaders/ShadersDefs.h"

#include "RenderingOp.h"
#include "Vertex.h"
#include <d3d9.h>
enum TransformationType
{
	TRANS_VIEWPROJ=0,
	TRANS_MODEL,

	TRANS_COUNT
};

class IRenderingCommandBlock;

#endif

#define MAX_COMMANDBLOCK_DEPTH				64
#define MAX_SHADERCONSTANT_SIZE				64


namespace Core
{

class RenderingCommandBlock;


#ifdef DEEXPORT
class Renderer : public IRenderer,
				 public TSingleton<Renderer>
{
	public:
	
		Renderer();
		~Renderer();

		bool Initialize(HWND hWnd, HINSTANCE hInst, int iWidth, int iHeight);
		bool Update();
		
		IDirect3DVertexBuffer9 *CreateVertexBuffer(void *pVertices,uint32 dwCount,VertexType _Type);
		IDirect3DIndexBuffer9 *CreateIndexBuffer(void *pIndices,uint32 dwCount,D3DFORMAT _Format);

		const Vector2i& GetScreenSize() const;

		const Vector2i& GetViewportPos() const;
		const Vector2i& GetViewportSize() const;
		void SetViewport(const Vector2i& vViewportPos, const Vector2i& vViewportSize);

		IDirect3DDevice9 *GetDevice() { return m_pDevice; };
		IDirect3D9 *GetD3D() { return m_pD3D; };

		IDirect3DVertexDeclaration9 *GetVertexDeclaration(VertexType _Type) { return m_pVertDecl[_Type]; }

		void Shutdown();
		void EnableClearingColorBuffer(bool bEnable) {}
		void EnableMultiThreading(bool bEnable);
							   
		bool AddView(HWND hWnd, int iWidth, int iHeight);
		bool RemoveView(HWND hWnd);
		bool SetView(HWND hWnd);
		HWND GetCurrentView() const;
		bool ResizeView(HWND hWnd, int iWidth, int iHeight);
		bool DrawImageInView(HTexture hTex, HWND hWnd);

		void EndScene(bool bForceExecute=true);
		void BeginScene();

		//			Queue

		void ClearRenderingBuffers();

		bool StartCommandBlock();
		IRenderingCommandBlock *EndCommandBlock();

		void ReleaseCommandBlock(IRenderingCommandBlock *pBlock);

		bool AddCommandBlock(IRenderingCommandBlock *pBlock);
		bool AddOp(const RenderingOp& op);

		//			Commandes

		bool SetStateBlock(IDirect3DStateBlock9 *pStateBlock)
			{ return AddOp(RenderingOp(RC_SETSTATEBLOCK,(void *)pStateBlock)); }
		bool SetRenderState(D3DRENDERSTATETYPE type, DWORD dwValue)
			{ return AddOp(RenderingOp(RC_SETRENDERSTATE,0,(uint32)type,dwValue)); }
		bool SetTexture(int iStage, IResource *pResource)
			{ return AddOp(RenderingOp(RC_SETTEXTURE,(void *)pResource,(uint32)iStage)); }
		bool SetShader(IResource *pResource)
			{ return AddOp(RenderingOp(RC_SETSHADER,(void *)pResource)); }
		
		bool SetTransformation(TransformationType type, const Matrix4& mTrans,const Matrix4 *pInverse=0, bool bInverse=true);
		bool SetVertexShaderConstant(uint32 dwReg, const float *fConstant, int iCount=1);

		bool SetVertexDeclaration(VertexType _Type)
			{ return AddOp(RenderingOp(RC_SETVERTEXDECLARATION,0,(uint32)_Type)); }
		bool ClearBuffer(uint32 dwBufferFlags, uint32 dwValue)
			{ return AddOp(RenderingOp(RC_CLEARBUFFER,0,dwBufferFlags, dwValue)); }

		bool SetVertexBuffer(IDirect3DVertexBuffer9 *pVBuffer,uint32 dwVertCount, uint32 dwVertSize)
			{ return AddOp(RenderingOp(RC_SETVERTEXBUFFER,(void *)pVBuffer,dwVertCount,dwVertSize)); }
		bool SetIndexBuffer(IDirect3DIndexBuffer9 *pIBuffer)
			{ return AddOp(RenderingOp(RC_SETINDEXBUFFER,(void *)pIBuffer)); }
		bool RenderStaticBuffers(D3DPRIMITIVETYPE type, uint32 dwOffset, uint32 dwCount)
			{ return AddOp(RenderingOp(RC_RENDERSTATICBUFFERS,(void *)type, dwOffset, dwCount)); }

		bool RenderDynamicBuffers(D3DPRIMITIVETYPE type, int iVertCount, void *pVertBuffer,
			VertexType VertType, int iIndCount, void *pInd, D3DFORMAT IndFormat);

		bool SetMaterial(D3DMATERIAL9 *pMat)
			{ return AddOp(RenderingOp(RC_SETMATERIAL,(void *)pMat)); }

#ifdef DEDEV
		bool HasExecErrors() const { return m_strErrorBuffer.length() > 0; }
		const std::string& GetErrorBuffer() const { return m_strErrorBuffer; }
#endif

		bool Execute();
		uint64 GetRenderingTime() const { return m_dwRenderingTime; }

	protected:

		class TransParam
		{
			public:
				
				TransParam() {}
				~TransParam() {}
				void Clear() {}

				Matrix4 mTrans;
				Matrix4 mInverse;
				bool bSetInverse;
				bool bBuildInverse;

				TransformationType _Type;
		};

		class ShaderConstantParam
		{
			public:

				ShaderConstantParam() {}
				~ShaderConstantParam() {}
				void Clear() {}

				float fConstant[MAX_SHADERCONSTANT_SIZE];
				int iReg;
				int iCount;
		};

		class RenderDynBuffersParam
		{
			public:

				RenderDynBuffersParam() {}
				~RenderDynBuffersParam() {}
				void Clear() {}

				uint32 dwVertOffset;
				uint32 dwVertCount;
				Core::VertexType VertType;

				uint32 dwIndOffset;
				uint32 dwIndCount;
				D3DFORMAT IndFormat;

				D3DPRIMITIVETYPE PrimitiveType;
		};

		bool NewFrame();

		bool ExecuteThread();

		bool ExecuteOp(const RenderingOp& op);

		bool ExecNone(const RenderingOp& op);
		bool ExecStateBlock(const RenderingOp& op);
		bool ExecRenderState(const RenderingOp& op);
		bool ExecSetTexture(const RenderingOp& op);
		bool ExecSetShader(const RenderingOp& op);
		bool ExecSetTransformation(const RenderingOp& op);
		bool ExecSetVertexShaderConstant(const RenderingOp& op);
		bool ExecSetVertexDeclaration(const RenderingOp& op);
		bool ExecClearBuffer(const RenderingOp& op);
		bool ExecSetVertexBuffer(const RenderingOp& op);
		bool ExecSetIndexBuffer(const RenderingOp& op);
		bool ExecRenderStaticBuffers(const RenderingOp& op);
		bool ExecRenderDynamicBuffers(const RenderingOp& op);
		bool ExecSetMaterial(const RenderingOp& op);

		typedef bool (Renderer::*ExecFunction)(const RenderingOp&);
		ExecFunction m_ExecTable[RC_COUNT];

		std::vector<RenderingOp> m_Buffers[2];
		TDynamicTab<byte> m_VertBuffers[2];
		TDynamicTab<byte> m_IndBuffers[2];
		TDynamicTab<TransParam> m_TransParamBuffers[2];
		TDynamicTab<ShaderConstantParam> m_ShaderConstantParamBuffers[2];
		TDynamicTab<RenderDynBuffersParam> m_RenderDynBuffersParamBuffers[2];

		int m_iCurrentExecBuffer;
		int m_iCurrentFillBuffer;

		RenderingCommandBlock *m_pCurrentBlock[MAX_COMMANDBLOCK_DEPTH];
		int m_iCurrentBlock;

		uint32 m_dwCurrentVertCount;			// pour les buffers statiques
		uint64 m_dwRenderingTime;

#ifdef DEDEV
		std::string m_strErrorBuffer;
		void AddErrorMsg(const char *szMsg,...);
#endif

		void BuildVertexDeclarations();
		void InitRenderState();
		bool ResetDevice();
	
		IDirect3D9 *m_pD3D;
		IDirect3DDevice9 *m_pDevice;
		D3DPRESENT_PARAMETERS m_DeviceParams;

		int m_iMultiSample;
		int m_iAnisotropic;
		int m_iShadersVersion;
		HINSTANCE m_hInstance;
		HWND m_hWnd;

		IDirect3DVertexDeclaration9 *m_pVertDecl[VX_COUNT];
		VertexType m_LastVertDecl;

		class View
		{
			public:

				View() : hWnd(0),vSize(-1,-1),vViewportPos(-1,-1), vViewportSize(-1,-1),
					pSwapChain(0),pZBuffer(0),pBackBuffer(0) {}
				View(HWND _hWnd, const Vector2i& _vSize,
					const Vector2i& _vViewportPos,
					const Vector2i& _vViewportSize) : hWnd(_hWnd), vSize(_vSize),
						vViewportPos(_vViewportPos),vViewportSize(_vViewportSize),
						pSwapChain(0), pZBuffer(0), pBackBuffer(0) {}
				View(const View& v) : hWnd(v.hWnd), vSize(v.vSize),vViewportPos(v.vViewportPos),
					vViewportSize(v.vViewportSize),pSwapChain(v.pSwapChain),
					pZBuffer(v.pZBuffer),pBackBuffer(v.pBackBuffer) {}

				const View& operator=(const View& v)
				{
					hWnd=v.hWnd;
					vSize=v.vSize;
					vViewportPos=v.vViewportPos;
					vViewportSize=v.vViewportSize;
					pSwapChain=v.pSwapChain;
					pZBuffer=v.pZBuffer;
					pBackBuffer=v.pBackBuffer;

					return *this;
				}

				HWND hWnd;
				Vector2i vSize;
				Vector2i vViewportPos;
				Vector2i vViewportSize;
				IDirect3DSwapChain9 *pSwapChain;
				IDirect3DSurface9 *pZBuffer;
				IDirect3DSurface9 *pBackBuffer;
		};

		std::vector<View> m_Views;
		int m_iCurrentView;
		bool m_bMultiThreadingEnable;

		int GetView(HWND hWnd);
};
#else
class Renderer : public ITask,
				 public TSingleton<Renderer>
{
	public:
	
		Renderer(const int iPriority, HINSTANCE hInst);
		~Renderer();

		bool Initialize();
		bool Update();
		
		bool OnSuspend();
		bool OnResume();
		
		bool Stop();
		
		bool BeginScene();
		bool EndScene();

		bool Restart();

		IDirect3DVertexBuffer9 *CreateVertexBuffer(void *pVertices,uint32 dwCount,VertexType _Type);
		IDirect3DIndexBuffer9 *CreateIndexBuffer(void *pIndices,uint32 dwCount,D3DFORMAT _Format);

		const Vector2i& GetScreenSize() const { return m_vScreenSize; };

		const Vector2i& GetViewportPos() const { return m_vViewportPos; };
		const Vector2i& GetViewportSize() const { return m_vViewportSize; };
		void SetViewport(const Vector2i& vViewportPos, const Vector2i& vViewportSize);

		bool IsWindowed() const { return m_bWindowed; };
		IDirect3DDevice9 *GetDevice() { return m_pDevice; };
		IDirect3D9 *GetD3D() { return m_pD3D; };

		void Shutdown();
		void EnableClearingColorBuffer(bool bEnable) { m_bClearColorBuffer=bEnable; }

		//			Queue

		void ClearRenderingBuffers();

		bool StartCommandBlock();
		IRenderingCommandBlock *EndCommandBlock();

		void ReleaseCommandBlock(IRenderingCommandBlock *pBlock);

		bool AddCommandBlock(IRenderingCommandBlock *pBlock);
		bool AddOp(const RenderingOp& op);

		//			Commandes

		bool SetStateBlock(IDirect3DStateBlock9 *pStateBlock)
			{ return AddOp(RenderingOp(RC_SETSTATEBLOCK,(void *)pStateBlock)); }
		bool SetRenderState(D3DRENDERSTATETYPE type, DWORD dwValue)
			{ return AddOp(RenderingOp(RC_SETRENDERSTATE,0,(uint32)type,dwValue)); }
		bool SetTexture(int iStage, IResource *pResource)
			{ return AddOp(RenderingOp(RC_SETTEXTURE,(void *)pResource,(uint32)iStage)); }
		bool SetShader(IResource *pResource)
			{ return AddOp(RenderingOp(RC_SETSHADER,(void *)pResource)); }
		
		bool SetTransformation(TransformationType type, const Matrix4& mTrans,const Matrix4 *pInverse=0, bool bInverse=true);
		bool SetVertexShaderConstant(uint32 dwReg, const float *fConstant, int iCount=1);

		bool SetVertexDeclaration(VertexType _Type)
			{ return AddOp(RenderingOp(RC_SETVERTEXDECLARATION,0,(uint32)_Type)); }
		bool ClearBuffer(uint32 dwBufferFlags, uint32 dwValue)
			{ return AddOp(RenderingOp(RC_CLEARBUFFER,0,dwBufferFlags, dwValue)); }

		bool SetVertexBuffer(IDirect3DVertexBuffer9 *pVBuffer,uint32 dwVertCount, uint32 dwVertSize)
			{ return AddOp(RenderingOp(RC_SETVERTEXBUFFER,(void *)pVBuffer,dwVertCount,dwVertSize)); }
		bool SetIndexBuffer(IDirect3DIndexBuffer9 *pIBuffer)
			{ return AddOp(RenderingOp(RC_SETINDEXBUFFER,(void *)pIBuffer)); }
		bool RenderStaticBuffers(D3DPRIMITIVETYPE type, uint32 dwOffset, uint32 dwCount)
			{ return AddOp(RenderingOp(RC_RENDERSTATICBUFFERS,(void *)type, dwOffset, dwCount)); }

		bool RenderDynamicBuffers(D3DPRIMITIVETYPE type, int iVertCount, void *pVertBuffer,
			VertexType VertType, int iIndCount, void *pInd, D3DFORMAT IndFormat);

		bool SetMaterial(D3DMATERIAL9 *pMat)
			{ return AddOp(RenderingOp(RC_SETMATERIAL,(void *)pMat)); }

#ifdef DEDEV
		bool HasExecErrors() const { return m_strErrorBuffer.length() > 0; }
		const std::string& GetErrorBuffer() const { return m_strErrorBuffer; }
#endif

		bool Execute();
		uint64 GetRenderingTime() const { return m_dwRenderingTime; }

	protected:

		class TransParam
		{
			public:
				
				TransParam() {}
				~TransParam() {}
				void Clear() {}

				Matrix4 mTrans;
				Matrix4 mInverse;
				bool bSetInverse;
				bool bBuildInverse;

				TransformationType _Type;
		};

		class ShaderConstantParam
		{
			public:

				ShaderConstantParam() {}
				~ShaderConstantParam() {}
				void Clear() {}

				float fConstant[MAX_SHADERCONSTANT_SIZE];
				int iReg;
				int iCount;
		};

		class RenderDynBuffersParam
		{
			public:

				RenderDynBuffersParam() {}
				~RenderDynBuffersParam() {}
				void Clear() {}

				uint32 dwVertOffset;
				uint32 dwVertCount;
				Core::VertexType VertType;

				uint32 dwIndOffset;
				uint32 dwIndCount;
				D3DFORMAT IndFormat;

				D3DPRIMITIVETYPE PrimitiveType;
		};

		bool NewFrame();

		bool ExecuteThread();

		bool ExecuteOp(const RenderingOp& op);

		bool ExecNone(const RenderingOp& op);
		bool ExecStateBlock(const RenderingOp& op);
		bool ExecRenderState(const RenderingOp& op);
		bool ExecSetTexture(const RenderingOp& op);
		bool ExecSetShader(const RenderingOp& op);
		bool ExecSetTransformation(const RenderingOp& op);
		bool ExecSetVertexShaderConstant(const RenderingOp& op);
		bool ExecSetVertexDeclaration(const RenderingOp& op);
		bool ExecClearBuffer(const RenderingOp& op);
		bool ExecSetVertexBuffer(const RenderingOp& op);
		bool ExecSetIndexBuffer(const RenderingOp& op);
		bool ExecRenderStaticBuffers(const RenderingOp& op);
		bool ExecRenderDynamicBuffers(const RenderingOp& op);
		bool ExecSetMaterial(const RenderingOp& op);

		typedef bool (Renderer::*ExecFunction)(const RenderingOp&);
		ExecFunction m_ExecTable[RC_COUNT];

		std::vector<RenderingOp> m_Buffers[2];
		TDynamicTab<byte> m_VertBuffers[2];
		TDynamicTab<byte> m_IndBuffers[2];
		TDynamicTab<TransParam> m_TransParamBuffers[2];
		TDynamicTab<ShaderConstantParam> m_ShaderConstantParamBuffers[2];
		TDynamicTab<RenderDynBuffersParam> m_RenderDynBuffersParamBuffers[2];

		int m_iCurrentExecBuffer;
		int m_iCurrentFillBuffer;

		RenderingCommandBlock *m_pCurrentBlock[MAX_COMMANDBLOCK_DEPTH];
		int m_iCurrentBlock;

		uint32 m_dwCurrentVertCount;			// pour les buffers statiques
		uint64 m_dwRenderingTime;

#ifdef DEDEV
		std::string m_strErrorBuffer;
		void AddErrorMsg(const char *szMsg,...);
#endif
	
		void BuildVertexDeclarations();
		void InitRenderState();
		bool ResetDevice();
		bool CreateDevice();
	
		IDirect3D9 *m_pD3D;
		IDirect3DDevice9 *m_pDevice;
		D3DPRESENT_PARAMETERS m_DeviceParams;

		Vector2i m_vScreenSize;
		Vector2i m_vViewportPos;
		Vector2i m_vViewportSize;

		bool m_bClearColorBuffer;
		bool m_bWindowed;
		int m_iMultiSample;
		int m_iAnisotropic;
		int m_iShadersVersion;
		HWND m_hWnd;
		HINSTANCE m_hInstance;

		IDirect3DVertexDeclaration9 *m_pVertDecl[VX_COUNT];
		VertexType m_LastVertDecl;
};
#endif

}

#define g_Renderer				Core::Renderer::GetSingletonRef()

#endif
