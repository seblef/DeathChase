
#ifndef _IRENDERER_H_
#define _IRENDERER_H_

#include <d3d9.h>
#include "../Core/Vertex.h"
#include "../Core/Handles.h"
#include "../Core/RenderingOp.h"

#include "../Exec/Materials/Shaders/ShadersDefs.h"


enum TransformationType
{
	TRANS_VIEWPROJ=0,
	TRANS_MODEL,

	TRANS_COUNT
};

#define TRANSFORMATIONTYPE_DEFINED

class IRenderingCommandBlock;

class IRenderer
{
	public:

		IRenderer() {}
		~IRenderer() {}

		virtual bool Initialize(HWND hWnd, HINSTANCE hInst, int iHeight, int iWidth)=0;

		virtual void EndScene(bool bForceExecute=true)=0;
		virtual void BeginScene()=0;
		
		virtual IDirect3DVertexBuffer9 *CreateVertexBuffer(void *pVertices,uint32 dwCount,Core::VertexType _Type)=0;
		virtual IDirect3DIndexBuffer9 *CreateIndexBuffer(void *pIndices,uint32 dwCount,D3DFORMAT _Format)=0;

		virtual const Core::Vector2i& GetScreenSize() const=0;

		virtual const Core::Vector2i& GetViewportPos() const=0;
		virtual const Core::Vector2i& GetViewportSize() const=0;
		virtual void SetViewport(const Core::Vector2i& vViewportPos, const Core::Vector2i& vViewportSize)=0;

		virtual IDirect3DDevice9 *GetDevice()=0;

		virtual IDirect3DVertexDeclaration9 *GetVertexDeclaration(Core::VertexType _Type)=0;

		virtual void Shutdown()=0;

		// Gestion des vues
		
		virtual bool AddView(HWND hWnd, int iWidth, int iHeight)=0;
		virtual bool RemoveView(HWND hWnd)=0;
		virtual bool SetView(HWND hWnd)=0;
		virtual HWND GetCurrentView() const=0;
		virtual bool ResizeView(HWND hWnd, int iWidth, int iHeight)=0;
		virtual bool DrawImageInView(Core::HTexture hTex, HWND hWnd)=0;

		//			Queue

		virtual bool StartCommandBlock()=0;
		virtual IRenderingCommandBlock *EndCommandBlock()=0;

		virtual void ReleaseCommandBlock(IRenderingCommandBlock *pBlock)=0;

		virtual bool AddCommandBlock(IRenderingCommandBlock *pBlock)=0;
		virtual bool AddOp(const Core::RenderingOp& op)=0;

		//			Commandes

		virtual bool SetStateBlock(IDirect3DStateBlock9 *pStateBlock)=0;
		virtual bool SetRenderState(D3DRENDERSTATETYPE type, DWORD dwValue)=0;
		virtual bool SetTransformation(TransformationType type, const Core::Matrix4& mTrans,
			const Core::Matrix4 *pInverse=0, bool bInverse=true)=0;
		virtual bool SetVertexShaderConstant(uint32 dwReg, const float *fConstant, int iCount=1)=0;
		virtual bool SetVertexDeclaration(Core::VertexType _Type)=0;
		virtual bool ClearBuffer(uint32 dwBufferFlags, uint32 dwValue)=0;
		virtual bool SetVertexBuffer(IDirect3DVertexBuffer9 *pVBuffer,uint32 dwVertCount, uint32 dwVertSize)=0;
		virtual bool SetIndexBuffer(IDirect3DIndexBuffer9 *pIBuffer)=0;
		virtual bool RenderStaticBuffers(D3DPRIMITIVETYPE type, uint32 dwOffset, uint32 dwCount)=0;
		virtual bool RenderDynamicBuffers(D3DPRIMITIVETYPE type, int iVertCount, void *pVertBuffer,
			Core::VertexType VertType, int iIndCount, void *pInd, D3DFORMAT IndFormat)=0;
		virtual bool SetMaterial(D3DMATERIAL9 *pMat)=0;
};

#endif