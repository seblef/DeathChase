
#ifndef _RENDERER2D_H_
#define _RENDERER2D_H_

#include "TSingleton.h"
#include "TDynamicTab.h"
#include "Vertex.h"

#ifdef DEEXPORT
#include "../Export/IRenderer2D.h"
#else
#include "Rect.h"
#include "Handles.h"
#endif

struct IDirect3DDevice9;

namespace Core
{

#ifdef DEEXPORT
class Renderer2D : public IRenderer2D, public TSingleton<Renderer2D>
#else
class Renderer2D : public TSingleton<Renderer2D>
#endif
{
	public:

		Renderer2D(IDirect3DDevice9 *pDevice);
		~Renderer2D();

		void Start();
		void End();

		void DrawRectangle(int x1, int y1, int x2, int y2, const Color32& cColor);
		void DrawRectangle(const Color32& cColor, const Rect& rect, const Rect *clip=0);
		void DrawRectangle(const Rect& pos,
			Color32 lu, Color32 ru, Color32 ld, Color32 rd, const Rect *clip=0);
		void Draw2DImage(HTexture hTex, int x1, int y1, int x2, int y2,
			float u1=0.0f,float v1=0.0f, float u2=1.0f, float v2=1.0f);
		void Draw2DImage(HTexture hTex,const Vector2i& vPos,
			const Rect& sRect,
			const Rect* pClip,
			Color32 cColor,
			bool bUseTextureAlpha);

		void Execute();
		void ShutDown();

		bool BeginRestart();
		bool EndRestart(IDirect3DDevice9 *pDevice);

	private:

		void BuildIdxBuffer(int iCount=128);

		IDirect3DDevice9 *m_pDevice;

		HShader m_hFillShader;
		HShader m_hTextureShader;

		TDynamicTab<Vertex2DPD> m_FillBuffer;
		TDynamicTab<Vertex2DPDT> m_TexBuffer;
		TDynamicTab<uint16> m_IdxBuffer;
		int m_iTriCreatedCount;

		bool m_bTextureEnable;
		HTexture m_hTexture;
		Vector2 m_vTextureSize;

};

}

#define g_Renderer2D			Core::Renderer2D::GetSingletonRef()

#endif