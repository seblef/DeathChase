
#include "ParticleEmitterStd.h"
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

TMemoryAllocator<ParticleEmitter_Linear> ParticleEmitter_Linear::m_sAllocator("ParticleEmitter_Linear");
TMemoryAllocator<ParticleEmitter_ExplosionRing> ParticleEmitter_ExplosionRing::m_sAllocator("ParticleEmitter_ExplosionRing");
TMemoryAllocator<ParticleEmitter_Debris> ParticleEmitter_Debris::m_sAllocator("ParticleEmitter_Debris");
TMemoryAllocator<ParticleEmitter_Spray> ParticleEmitter_Spray::m_sAllocator("ParticleEmitter_Spray");
TMemoryAllocator<ParticleEmitter_Smoke> ParticleEmitter_Smoke::m_sAllocator("ParticleEmitter_Smoke");
TMemoryAllocator<ParticleEmitter_Explosion> ParticleEmitter_Explosion::m_sAllocator("ParticleEmitter_Explosion");
TMemoryAllocator<ParticleEmitter_Precipitation> ParticleEmitter_Precipitation::m_sAllocator("ParticleEmitter_Precipitation");


void ParticleEmitter_Linear::Emit(const Vector3 &vPos, Particle *pParticle)
{
	RandomVector(pParticle->m_vPosition,m_vPos1,m_vPos2);
}

void ParticleEmitter_Linear::Emit(const Vector3 &vPos, Particle *pParticles, int iCount)
{
	Particle *pPart=pParticles;
	for(int i=0;i<iCount;i++,pPart++)
		RandomVector(pPart->m_vPosition,m_vPos1,m_vPos2);
}



void ParticleEmitter_ExplosionRing::Emit(const Vector3 &vPos, Particle *pParticle)
{
	float fAng=RAND * 360.0f;
	float fSpeed=45.0f;
	pParticle->m_vVelocity.x=Matrix4::TSin[(int)fAng] * fSpeed;
	pParticle->m_vVelocity.z=Matrix4::TCos[(int)fAng] * fSpeed;
	pParticle->m_vVelocity.y=0.0f;

	pParticle->m_vPosition=vPos;
}

void ParticleEmitter_ExplosionRing::Emit(const Vector3 &vPos, Particle *pParticles, int iCount)
{
	Particle *pPart=pParticles;
	for(int i=0;i<iCount;i++,pPart++)
	{
		float fAng=RAND * 360.0f;
		float fSpeed=45.0f;
		pPart->m_vVelocity.x=Matrix4::TSin[(int)fAng] * fSpeed;
		pPart->m_vVelocity.z=Matrix4::TCos[(int)fAng] * fSpeed;
		pPart->m_vVelocity.y=0.0f;

		pPart->m_vPosition=vPos;
	}
}




void ParticleEmitter_Debris::Emit(const Vector3& vPos, Particle *pParticle)
{
	float fDir=RAND * 360.0f;
	float fSpeed=RAND * 2.5f + 1.0f;
	float fHeight=RAND * 5.0f + 3.0f;

	pParticle->m_vVelocity.x=Matrix4::TSin[(int)fDir] * fSpeed;
	pParticle->m_vVelocity.y=fHeight;
	pParticle->m_vVelocity.z=Matrix4::TCos[(int)fDir] * fSpeed;
	pParticle->m_vPosition=vPos;
}

void ParticleEmitter_Debris::Emit(const Vector3 &vPos, Particle *pParticles, int iCount)
{
	Particle *pPart=pParticles;
	for(int i=0;i<iCount;i++,pPart++)
	{
		float fDir=RAND * 360.0f;
		float fSpeed=RAND * 2.5f + 1.0f;
		float fHeight=RAND * 5.0f + 3.0f;

		pPart->m_vVelocity.x=Matrix4::TSin[(int)fDir] * fSpeed;
		pPart->m_vVelocity.y=fHeight;
		pPart->m_vVelocity.z=Matrix4::TCos[(int)fDir] * fSpeed;
		pPart->m_vPosition=vPos;
	}
}




void ParticleEmitter_Spray::Emit(const Vector3& vPos, Particle *pParticle)
{
	float fRot=RAND * 360.0f;
	float x=Matrix4::TSin[(int)fRot];
	float y=Matrix4::TCos[(int)fRot];

	pParticle->m_vPosition=vPos;
	pParticle->m_vPosition.x+=(x*m_fWidth*RAND);
	pParticle->m_vPosition.z+=(y*m_fWidth*RAND);

	pParticle->m_vVelocity.x=(x*RAND) / 10.0f;
	pParticle->m_vVelocity.y=0.5f * RAND + 0.3f;
	pParticle->m_vVelocity.z=(y*RAND) / 10.0f;
}

void ParticleEmitter_Spray::Emit(const Vector3 &vPos, Particle *pParticles, int iCount)
{
	Particle *pPart=pParticles;
	for(int i=0;i<iCount;i++,pPart++)
	{
		float fRot=RAND * 360.0f;
		float x=Matrix4::TSin[(int)fRot];
		float y=Matrix4::TCos[(int)fRot];

		pPart->m_vPosition=vPos;
		pPart->m_vPosition.x+=(x*m_fWidth*RAND);
		pPart->m_vPosition.z+=(y*m_fWidth*RAND);

		pPart->m_vVelocity.x=(x*RAND) / 10.0f;
		pPart->m_vVelocity.y=0.5f * RAND + 0.3f;
		pPart->m_vVelocity.z=(y*RAND) / 10.0f;
	}
}




void ParticleEmitter_Smoke::Emit(const Vector3& vPos, Particle *pParticle)
{
	pParticle->m_vPosition=vPos;
}

void ParticleEmitter_Smoke::Emit(const Vector3& vPos, Particle *pParticles, int iCount)
{
	Particle *pPart=pParticles;
	for(int i=0;i<iCount;i++,pPart++)
		pPart->m_vPosition=vPos;
}




void ParticleEmitter_Explosion::Emit(const Vector3 &vPos, Particle *pParticle)
{
	const float fRotXY=(RAND * 360.0f);
	const float fRotXZ=(RAND * 360.0f);
	const float fCosRot=Matrix4::TCos[(int)fRotXZ];

	pParticle->m_vVelocity.x=sinf(fRotXY) * fCosRot;
	pParticle->m_vVelocity.z=cosf(fRotXY) * fCosRot;
	pParticle->m_vVelocity.y=sinf(fRotXZ);
	pParticle->m_vVelocity*=5.0f;

	pParticle->m_vPosition=vPos;
}

void ParticleEmitter_Explosion::Emit(const Vector3 &vPos, Particle *pParticles, int iCount)
{
	Particle *pPart=pParticles;
	for(int i=0;i<iCount;i++,pPart++)
	{
		const float fRotXY=(RAND * 360.0f);
		const float fRotXZ=(RAND * 360.0f);
		const float fCosRot=Matrix4::TCos[(int)fRotXZ];

		pPart->m_vVelocity.x=sinf(fRotXY) * fCosRot;
		pPart->m_vVelocity.z=cosf(fRotXY) * fCosRot;
		pPart->m_vVelocity.y=sinf(fRotXZ);
		pPart->m_vVelocity*=2.5f;

		pPart->m_vPosition=vPos;
	}
}



void ParticleEmitter_Precipitation::Emit(const Vector3& vPos, Particle *pParticle)
{
	pParticle->m_vPosition.x=vPos.x + RAND*400.0f - 200.0f;
	pParticle->m_vPosition.z=vPos.z + RAND*400.0f - 200.0f;
	pParticle->m_vPosition.y=200.0f;
}

void ParticleEmitter_Precipitation::Emit(const Vector3& vPos, Particle *pParticles, int iCount)
{
	Particle *pPart=pParticles;
	for(int i=0;i<iCount;i++,pPart++)
	{
		pPart->m_vPosition.x=vPos.x + RAND*400.0f - 200.0f;
		pPart->m_vPosition.z=vPos.z + RAND*400.0f - 200.0f;
		pPart->m_vPosition.y=200.0f;
	}
}

}