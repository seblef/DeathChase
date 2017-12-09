
#include "Explosion.h"
#include "../Core/MaterialMgr.h"
#include "../Core/ParticleEngine.h"
#include "../Core/ParticleSystem.h"
#include "../Core/Camera.h"
#include "../Core/MaterialMgr.h"
#include "../Core/Renderer.h"
#include "../Core/Sound.h"
#include "../Core/SoundVirtualSource.h"


namespace Game
{

Core::TMemoryAllocator<Explosion> Explosion::m_sAllocator("Explosion");

Explosion::~Explosion()
{
	Clear();
}

bool Explosion::Create(const Core::Vector3 &vPos, const float fSize, float fLifeTime,
					   Core::HMaterial hMat, Core::HSound hSound,
					   bool bSmoke, bool bDebris, bool bSplash,
					   const char *szSmokeTemplate, const char *szSplashTemplate, const char *szDebrisTemplate)
{
	if(hMat.IsNull()) return false;

	m_vPosition=vPos;
	m_fSize=fSize;
	m_fLifeTime=fLifeTime;
	m_hMaterial=hMat;
	m_hSound=hSound;

	if(bSmoke)
	{
		m_pSmokeSystem=g_ParticleEngine.CreateSystem(szSmokeTemplate ? szSmokeTemplate : "DefaultExplosionSmoke");
		if(m_pSmokeSystem)
		{
			m_pSmokeSystem->SetPosition(Core::Vector3(vPos.x,vPos.y+fSize*0.5f,vPos.z));
			g_ParticleEngine.PlaySystem(m_pSmokeSystem,true);
		}
	}

	if(bDebris)
	{
		m_pDebrisSystem=g_ParticleEngine.CreateSystem(szDebrisTemplate ? szDebrisTemplate : "DefaultExplosionDebris");
		if(m_pDebrisSystem)
		{
			m_pDebrisSystem->SetPosition(vPos);
			g_ParticleEngine.PlaySystem(m_pDebrisSystem,true);
		}
	}

	if(bSplash)
	{
		m_pSplashSystem=g_ParticleEngine.CreateSystem(szSplashTemplate ? szSplashTemplate : "DefaultExplosionSplash");
		if(m_pSplashSystem)
		{
			m_pSplashSystem->SetPosition(vPos);
			g_ParticleEngine.PlaySystem(m_pSplashSystem,true);
		}
	}

	Core::SoundVirtualSource *pSource=g_Sound.Play(m_hSound,0);
	
	if(pSource)
	{
		pSource->ActualPlay();
		pSource->SetGain(1.0f);
		pSource->SetPosition(m_vPosition);
		pSource->SetRelative();
	}

	m_fTime=0.0f;

	return true;
}

void Explosion::Update(float fTime)
{
	m_fTime+=fTime;

	if(m_fTime >= m_fLifeTime)
	{
		if(m_pSmokeSystem) m_pSmokeSystem->StopEmission();
		if(m_pDebrisSystem) m_pDebrisSystem->StopEmission();
		if(m_pSplashSystem) m_pSplashSystem->StopEmission();

		m_pSmokeSystem=0;
		m_pDebrisSystem=0;
		m_pSplashSystem=0;

		return;
	}
}

void Explosion::Draw(Core::Camera *pCamera)
{
	if(m_fTime >= m_fLifeTime) return;

	Core::VertexPDT v[4];
	const uint16 ind[6]={0,1,2,2,1,3};

	v[0].vUV.x=1.0f; v[0].vUV.y=1.0f;
	v[1].vUV.x=0.0f; v[1].vUV.y=1.0f;
	v[2].vUV.x=1.0f; v[2].vUV.y=0.0f;
	v[3].vUV.x=0.0f; v[3].vUV.y=0.0f;

	const Core::Vector3& vX(pCamera->GetXAxis());
	const Core::Vector3& vY(pCamera->GetYAxis());

	Core::Color32 cColor; cColor.dwRGB=0xffffffff;

	Core::Vector3 vPos(m_vPosition+vY*m_fSize*0.5f);

	v[0].vPos=vPos + (vX+vY) * m_fSize;
	v[1].vPos=vPos + (vY-vX) * m_fSize;
	v[2].vPos=vPos + (vX-vY) * m_fSize;
	v[3].vPos=vPos - (vX+vY) * m_fSize;
	v[0].cColor=v[1].cColor=v[2].cColor=v[3].cColor=cColor;

	Core::Material *pMat=(Core::Material *)g_MaterialMgr.GetMaterial(m_hMaterial);
	if(pMat)
	{
		pMat->Set((uint32)(m_fTime * (float)pMat->GetTextureAnimTime(0) / m_fLifeTime));
		g_Renderer.SetVertexDeclaration(Core::VX_PDT);
		g_Renderer.SetTransformation(TRANS_MODEL,Core::Matrix4::Identity,0,false);
		g_Renderer.RenderDynamicBuffers(D3DPT_TRIANGLELIST,4,v,Core::VX_PDT,6,(void *)ind,D3DFMT_INDEX16);
	}
}

void Explosion::Clear()
{
	if(m_pSmokeSystem) m_pSmokeSystem->StopEmission();
	if(m_pDebrisSystem) m_pDebrisSystem->StopEmission();
	if(m_pSplashSystem) m_pSplashSystem->StopEmission();

	m_pSmokeSystem=0;
	m_pDebrisSystem=0;
	m_pSplashSystem=0;

	m_fTime=0.0f;
	m_fLifeTime=1.0f;
	m_fSize=1.0f;
}

}