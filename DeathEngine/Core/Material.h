
#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include "Handles.h"
#include "IResource.h"
#include "../Dev/MemDebugger.h"
#include "RenderingPassDefs.h"
#include "Color.h"
#include <d3d9.h>
#include <vector>
#include <map>

#ifdef DEEXPORT
#include "../Export/IMaterial.h"
#else
#define IMaterial			Core::Material
#endif

#define MAX_TEXTURE_STAGES						8


namespace Core
{

#ifdef DEEXPORT
class Material : public IMaterial, public IResource
#else
class Material : public IResource
#endif
{
	public:

		Material(IDirect3DDevice9 *pDevice);
		~Material();

		bool Create(const char *szName);
		bool Load(const char *szFileName, const char *szPath);
		bool Save(const char *szFileName, const char *szPath, const bool bSaveTextures=false);
		bool Set(const uint32 dwTime=0);
		void Release();

		const char *GetName() const { return m_szName; }
		void SetName(const char *szName);

		HShader GetShader(int iPass) const { return m_hShader[iPass]; }
		void SetShader(const int iPass, const HShader hShader) { m_hShader[iPass]=hShader; }

		HTexture GetTexture(const int iStage=0, const int iAnimNum=0);
		void SetTexture(HTexture hTex, const int iStage=0, const int iAnimNum=0);
		void AddTexture(HTexture hTex, const int iStage=0);
		void RemoveTexture(const int iStage=0, const int iAnimNum=0);
		
		float GetTextureAnimFrequency(const int iStage=0) const;
		uint32 GetTextureAnimTime(const int iStage=0) const;
		void SetTextureAnimFrequency(const float fFreq, const int iStage=0);
		void SetTextureAnimTime(const uint32 dwTime, const int iStage=0);

		bool IsAnimated() const { return m_bAnimated; }
		uint32 GetAnimTime() const { return m_dwMaxAnimTime; }
		void SetAnimated(const bool bAnimated) { m_bAnimated=bAnimated; }

		BOOL IsAlphaBlendEnable() const { return m_bBlend; }
		D3DBLEND GetSrcBlend() const { return m_SrcBlend; }
		D3DBLEND GetDstBlend() const { return m_DstBlend; }

		void SetAlphaBlendEnable(const BOOL bEnable);
		void SetSrcBlend(D3DBLEND SrcBlend);
		void SetDstBlend(D3DBLEND DstBlend);

		BOOL GetZWriteEnable() const { return m_bZWriteEnable; }
		void SetZWriteEnable(BOOL bEnable) { m_bZWriteEnable=bEnable; }

		D3DCMPFUNC GetDepthFnc() const { return m_DepthFnc; }
		void SetDepthFnc(const D3DCMPFUNC DepthFnc);

		D3DCMPFUNC GetStencilFnc() const { return m_StencilFnc; }
		void SetStencilFnc(const D3DCMPFUNC StencilFnc);

		D3DCULL GetCullMode() const { return m_CullMode; }
		void SetCullMode(const D3DCULL CullMode);

		const Color32f& GetDiffuseColor() const { return m_cDiffuse; }
		const Color32f& GetSpecularColor() const { return m_cSpecular; }
		const Color32f& GetEmissiveColor() const { return m_cEmissive; }

		void SetDiffuseColor(const Color32f& cDiffuse);
		void SetSpecularColor(const Color32f& cSpecular);
		void SetEmissiveColor(const Color32f& cEmissive);

		bool IsDiffuseEnable() const;
		bool IsSpecularEnable() const;
		bool IsEmissiveEnable() const;

		void DisableDiffuse();
		void DisableSpecular();
		void DisableEmissive();

		float GetShininess() const { return m_fShininess; }
		void SetShininess(const float fShininess);

		bool CastShadows() const { return m_bCastShadows; }
		void SetCastShadows(const bool bShadows) { m_bCastShadows=bShadows; }

		bool ReceiveShadows() const { return m_bReceiveShadows; }
		void SetReceiveShadows(bool bShadows) { m_bReceiveShadows=bShadows; }

		bool NoDraw() const { return m_bNoDraw; }
		void SetNoDraw(const bool bNoDraw) { m_bNoDraw=bNoDraw; }

		bool SameAs(IMaterial *pMat);

		static void SetRenderingPass(int iPass) { m_siRenderingPass=iPass; }

		MEMDEBUG_IMPL
		MEMDEBUG_CLASSNAME("Material")
		MEMDEBUG_CLASSSIZE(sizeof(Material))

	protected:

		inline void SetTexture(const int iStage,const uint32 dwTime);
		bool CompileMat();
		bool m_bMustRecompile;

		Color32f m_cDiffuse;
		Color32f m_cSpecular;
		Color32f m_cEmissive;
		float m_fShininess;
		uint32 m_dwColorFlags;

		BOOL m_bBlend;
		D3DBLEND m_SrcBlend;
		D3DBLEND m_DstBlend;
		BOOL m_bZWriteEnable;

		D3DCMPFUNC m_DepthFnc;
		D3DCMPFUNC m_StencilFnc;

		D3DCULL m_CullMode;

		bool m_bNoDraw,m_bCastShadows,m_bReceiveShadows;

		typedef std::vector<HTexture> TextureVector;
		TextureVector m_Stages[MAX_TEXTURE_STAGES];
		float m_fFrequency[MAX_TEXTURE_STAGES];
		uint32 m_dwAnimTimes[MAX_TEXTURE_STAGES];
		uint32 m_dwMaxAnimTime;

		HShader m_hShader[RENDERINGPASS_COUNT];

		bool m_bAnimated;

		D3DMATERIAL9 m_Mat;
		IDirect3DDevice9 *m_pDevice;

		char m_szName[64];

		static int m_siRenderingPass;
};

}

#endif