
#include "ParticleSystem.h"
#include "Particle.h"

namespace Core
{

static inline void RandomVector(Vector3& vRet,
						 const Vector3& vStart,
						 const Vector3& vEnd)
{
	vRet.x=vStart.x + ((vEnd.x - vStart.x) * RAND);
	vRet.y=vStart.y + ((vEnd.y - vStart.y) * RAND);
	vRet.z=vStart.z + ((vEnd.z - vStart.z) * RAND);
}
static inline void RandomCounterVector(Vector3& vRet,
								const Vector3& vCurrent,
								const Vector3& vStart,
								const Vector3& vEnd,
								const float fLife)
{
	RandomVector(vRet,vStart,vEnd);
	vRet-=vCurrent;
	vRet/=fLife;
}
static inline float RandomScalar(const float fStart, const float fEnd)
{
	return fStart + ((fEnd - fStart) * RAND);
}
static inline float RandomCounterScalar(const float fCurrent,
										const float fStart,
										const float fEnd,
										const float fLife)
{
	float res=RandomScalar(fStart,fEnd);
	res-=fCurrent;
	return res / fLife;
}
static inline void RandomColor(Color32f &cRet,
							   const Color32f& cStart,
							   const Color32f& cEnd)
{
	cRet.fa=cStart.fa + ((cEnd.fa - cStart.fa) * RAND);
	cRet.fr=cStart.fr + ((cEnd.fr - cStart.fr) * RAND);
	cRet.fg=cStart.fg + ((cEnd.fg - cStart.fg) * RAND);
	cRet.fb=cStart.fb + ((cEnd.fb - cStart.fb) * RAND);
}
static inline void RandomCounterColor(Color32f& cRet,
									  const Color32f& cCurrent,
									  const Color32f& cStart,
									  const Color32f& cEnd,
									  const float fLife)
{
	RandomColor(cRet,cStart,cEnd);
	cRet-=cCurrent;
	cRet*=1.0f / fLife;
}


TMemoryAllocator<ParticleSystem> ParticleSystem::m_sAllocator("ParticleSystem");


ParticleSystem::ParticleSystem() : m_pParticles(0), m_iParticlesCount(0), m_fTime(0.0f),m_fBBoxTime(0.11f),
	m_bFinished(false),	m_vPosition(Core::Vector3::NullVector), m_bStopEmission(false)
{
}

ParticleSystem::~ParticleSystem()
{
	if(m_pParticles)
		delete[] m_pParticles;

	if(m_Template.m_pEmitter) m_Template.m_pEmitter->Delete();
	if(m_Template.m_pRenderer) m_Template.m_pRenderer->Delete();
}

void ParticleSystem::Clear()
{
	m_FreeIndex.clear();
	for(int i=m_iParticlesCount-1;i>=0;i--)
		m_FreeIndex.push_back(i);

	if(m_Template.m_pEmitter) m_Template.m_pEmitter->Delete();
	if(m_Template.m_pRenderer) m_Template.m_pRenderer->Delete();

	m_Template.m_pEmitter=0;
	m_Template.m_pRenderer=0;

	m_bStopEmission=false;
	m_bFinished=false;
}

bool ParticleSystem::Create(const ParticleSystemTemplate &t)
{
	m_Template=t;
	m_Template.m_pRenderer=t.m_pRenderer->New();
	m_Template.m_pEmitter=t.m_pEmitter->New();

	if(m_Template.m_iParticleCount > m_iParticlesCount)
	{
		if(m_pParticles)
			delete[] m_pParticles;

		m_pParticles=new Particle[m_Template.m_iParticleCount];
		m_iParticlesCount=m_Template.m_iParticleCount;

		m_FreeIndex.clear();
		for(int i=m_iParticlesCount-1;i>=0;i--)
		{
			m_pParticles[i].m_iFactoryIndex=i;
			m_FreeIndex.push_back(i);
		}
	}

	m_fTime=0.0f;

	return true;
}

bool ParticleSystem::Create(int iParticleCount,
							float fLife1, float fLife2,
							float fFriction1, float fFriction2,
							const Vector3 &vVelocity1, const Vector3 &vVelocity2,
							const Color32f &cStartColor1, const Color32f &cStartColor2,
							const Color32f &cEndColor1, const Color32f &cEndColor2,
							float fStartSize1, float fStartSize2,
							float fEndSize1, float fEndSize2,
							float fMass1, float fMass2,
							const Vector3 &vGravity,
							IParticleRenderer *pRenderer,
							IParticleEmitter *pEmitter,
							bool bEmitOnce, bool bEmitLoop, float fEmitSpeed)
{
	m_Template.m_fLife1=fLife1; m_Template.m_fLife2=fLife2;
	m_Template.m_fFriction1=fFriction1; m_Template.m_fFriction2=fFriction2;
	m_Template.m_vVelocity1=vVelocity1; m_Template.m_vVelocity2=vVelocity2;
	m_Template.m_cStartColor1=cStartColor1; m_Template.m_cStartColor2=cStartColor2;
	m_Template.m_cEndColor1=cEndColor1; m_Template.m_cEndColor2=cEndColor2;
	m_Template.m_fStartSize1=fStartSize1; m_Template.m_fStartSize2=fStartSize2;
	m_Template.m_fEndSize1=fEndSize1; m_Template.m_fEndSize2=fEndSize2;
	m_Template.m_fMass1=fMass1; m_Template.m_fMass2=fMass2;
	m_Template.m_vGravity=vGravity;
	m_Template.m_pRenderer=pRenderer->New();
	m_Template.m_pEmitter=pEmitter->New();
	m_Template.m_fEmitSpeed=fEmitSpeed;
	m_Template.m_dwFlags=0;
	m_Template.m_iParticleCount=iParticleCount;

	if(bEmitOnce) m_Template.m_dwFlags|=PARTICLE_EMITONCE;
	if(bEmitLoop) m_Template.m_dwFlags|=PARTICLE_EMITLOOP;

	if(m_Template.m_iParticleCount > m_iParticlesCount)
	{
		if(m_pParticles)
			delete[] m_pParticles;

		m_pParticles=new Particle[m_Template.m_iParticleCount];
		m_iParticlesCount=m_Template.m_iParticleCount;

		m_FreeIndex.clear();
		for(int i=m_iParticlesCount-1;i>=0;i--)
		{
			m_pParticles[i].m_iFactoryIndex=i;
			m_FreeIndex.push_back(i);
		}
	}

	m_fTime=0.0f;

	return (pRenderer!=0 && pEmitter!=0);
}

void ParticleSystem::Update(const float fTime)
{
	if(m_fTime==0.0f && (m_Template.m_dwFlags & PARTICLE_EMITONCE) && m_Template.m_pEmitter)
	{
		for(int i=0;i<m_iParticlesCount;i++)
		{
			CreateNewParticle(m_pParticles+i);
			m_Template.m_pEmitter->Emit(m_vPosition,m_pParticles+i);
		}

		m_FreeIndex.clear();
	}

	Particle *pPart=m_pParticles;

	bool bEnd=true;
	for(int i=0;i<m_iParticlesCount;i++,pPart++)
	{
		if(pPart->m_fLife > 0.0f)
		{
			pPart->Update(fTime);
			if(pPart->m_fLife <= 0.0f)
				m_FreeIndex.push_back(pPart->m_iFactoryIndex);
			else
				bEnd=false;
		}
	}

	m_fTime+=fTime;
	m_fBBoxTime+=fTime;

	if(m_bStopEmission && bEnd)
	{
		m_bFinished=true;
		return;
	}
	else if(m_Template.m_dwFlags & PARTICLE_EMITONCE)
	{
		if(bEnd)
		{
			if(m_Template.m_dwFlags & PARTICLE_EMITLOOP)
			{
				for(int i=0;i<m_iParticlesCount;i++)
				{
					CreateNewParticle(m_pParticles+i);
					m_Template.m_pEmitter->Emit(m_vPosition,m_pParticles+i);
				}

				m_FreeIndex.clear();
			}
			else
			{
				m_bFinished=true;
				return;
			}
		}
	}
	else if(!m_bStopEmission)
	{
		const float fDeltaTime=1.0f * KINV(m_Template.m_fEmitSpeed);

		while(m_fTime > fDeltaTime)
		{
			if(m_FreeIndex.size() > 0 && m_Template.m_pEmitter)
			{
				int iIndex=m_FreeIndex.back();
				m_FreeIndex.pop_back();

				CreateNewParticle(m_pParticles+iIndex);
				m_Template.m_pEmitter->Emit(m_vPosition,m_pParticles+iIndex);

				m_bFinished=false;
			}

			m_fTime-=fDeltaTime;
		}
	}

	if(m_fBBoxTime >= 0.1f)
	{
		m_fBBoxTime-=0.1f;
		if(!bEnd)
		{
			m_BBox.Init();
			pPart=m_pParticles;

			for(int i=0;i<m_iParticlesCount;i++,pPart++)
				m_BBox.Adjust(pPart->m_vPosition);
		}
	}
}

void ParticleSystem::CreateNewParticle(Particle *pParticle)
{
	pParticle->m_fLife=RandomScalar(m_Template.m_fLife1,m_Template.m_fLife2);
	pParticle->m_fMass=RandomScalar(m_Template.m_fMass1,m_Template.m_fMass2);
	pParticle->m_fFriction=RandomScalar(m_Template.m_fFriction1,m_Template.m_fFriction2);
	RandomVector(pParticle->m_vVelocity,m_Template.m_vVelocity1,m_Template.m_vVelocity2);
	RandomColor(pParticle->m_cColor,m_Template.m_cStartColor1,m_Template.m_cStartColor2);
	RandomCounterColor(pParticle->m_cColorCounter,pParticle->m_cColor,
		m_Template.m_cEndColor1,m_Template.m_cEndColor2,pParticle->m_fLife);
	pParticle->m_fSize=RandomScalar(m_Template.m_fStartSize1,m_Template.m_fStartSize2);
	pParticle->m_fSizeCounter=RandomCounterScalar(pParticle->m_fSize,
		m_Template.m_fEndSize1,m_Template.m_fEndSize2,pParticle->m_fLife);
	pParticle->m_vGravity=m_Template.m_vGravity;
}

void ParticleSystem::Draw(Camera *pCamera)
{
	if(m_Template.m_pRenderer)
		m_Template.m_pRenderer->Draw(m_pParticles,m_iParticlesCount,pCamera);
}

}