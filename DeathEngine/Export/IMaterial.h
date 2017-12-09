
#ifndef _IMATERIAL_H_
#define _IMATERIAL_H_

#include "../Core/RenderingPassDefs.h"
#include "../Core/Handles.h"
#include "../Core/Color.h"
#include <d3d9.h>

class IMaterialInstance
{
	public:

		IMaterialInstance() {}
		virtual ~IMaterialInstance() {}

		virtual bool Set()=0;
};

class IMaterial
{
	public:

		IMaterial() {}
		virtual ~IMaterial() {}

		virtual bool Create(const char *szName)=0;
		virtual bool Load(const char *szFileName, const char *szPath)=0;
		virtual bool Save(const char *szFileName, const char *szPath, const bool bSaveTextures=false)=0;
		virtual bool Set(const uint32 dwTime=0)=0;
		virtual void Release()=0;

		virtual const char *GetName() const=0;
		virtual void SetName(const char *szName)=0;

		virtual Core::HShader GetShader(int iPass) const=0;
		virtual void SetShader(const int iPass, const Core::HShader hShader)=0;

		virtual Core::HTexture GetTexture(const int iStage=0, const int iAnimNum=0)=0;
		virtual void SetTexture(Core::HTexture hTex, const int iStage=0, const int iAnimNum=0)=0;
		virtual void AddTexture(Core::HTexture hTex, const int iStage=0)=0;
		virtual void RemoveTexture(const int iStage=0, const int iAnimNum=0)=0;
		
		virtual float GetTextureAnimFrequency(const int iStage=0) const=0;
		virtual uint32 GetTextureAnimTime(const int iStage=0) const=0;
		virtual void SetTextureAnimFrequency(const float fFreq, const int iStage=0)=0;
		virtual void SetTextureAnimTime(const uint32 dwTime, const int iStage=0)=0;

		virtual bool IsAnimated() const=0;
		virtual uint32 GetAnimTime() const=0;
		virtual void SetAnimated(const bool bAnimated)=0;

		virtual BOOL IsAlphaBlendEnable() const=0;
		virtual D3DBLEND GetSrcBlend() const=0;
		virtual D3DBLEND GetDstBlend() const=0;

		virtual BOOL GetZWriteEnable() const=0;
		virtual void SetZWriteEnable(BOOL bEnable)=0;

		virtual void SetAlphaBlendEnable(const BOOL bEnable)=0;
		virtual void SetSrcBlend(D3DBLEND SrcBlend)=0;
		virtual void SetDstBlend(D3DBLEND DstBlend)=0;

		virtual D3DCMPFUNC GetDepthFnc() const=0;
		virtual void SetDepthFnc(const D3DCMPFUNC DepthFnc)=0;

		virtual D3DCMPFUNC GetStencilFnc() const=0;
		virtual void SetStencilFnc(const D3DCMPFUNC StencilFnc)=0;

		virtual D3DCULL GetCullMode() const=0;
		virtual void SetCullMode(const D3DCULL CullMode)=0;

		virtual const Core::Color32f& GetDiffuseColor() const=0;
		virtual const Core::Color32f& GetSpecularColor() const=0;
		virtual const Core::Color32f& GetEmissiveColor() const=0;

		virtual void SetDiffuseColor(const Core::Color32f& cDiffuse)=0;
		virtual void SetSpecularColor(const Core::Color32f& cSpecular)=0;
		virtual void SetEmissiveColor(const Core::Color32f& cEmissive)=0;

		virtual bool IsDiffuseEnable() const=0;
		virtual bool IsSpecularEnable() const=0;
		virtual bool IsEmissiveEnable() const=0;

		virtual void DisableDiffuse()=0;
		virtual void DisableSpecular()=0;
		virtual void DisableEmissive()=0;

		virtual float GetShininess() const=0;
		virtual void SetShininess(const float fShininess)=0;

		virtual bool CastShadows() const=0;
		virtual void SetCastShadows(const bool bShadows)=0;

		virtual bool ReceiveShadows() const=0;
		virtual void SetReceiveShadows(bool bShadows)=0;

		virtual bool NoDraw() const=0;
		virtual void SetNoDraw(const bool bNoDraw)=0;

		virtual bool SameAs(IMaterial *pMat)=0;
};

#endif