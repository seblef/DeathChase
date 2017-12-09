
#ifndef _PARTICLESYSTEMTEMPLATE_H_
#define _PARTICLESYSTEMTEMPLATE_H_

#include "Vector.h"
#include "Color.h"

namespace Core
{

class IParticleEmitter;
class IParticleRenderer;
class ParticleSystem;

#define PARTICLE_EMITONCE			1
#define PARTICLE_EMITLOOP			2

class ParticleSystemTemplate
{
	public:

		ParticleSystemTemplate() : m_pRenderer(0), m_pEmitter(0), m_dwFlags(0) {}
		~ParticleSystemTemplate() {}

		const ParticleSystemTemplate& operator=(const ParticleSystemTemplate& t)
		{
			memcpy(this,&t,sizeof(ParticleSystemTemplate));
			return *this;
		}

		float m_fLife1,m_fLife2;
		float m_fFriction1, m_fFriction2;
		Vector3 m_vVelocity1, m_vVelocity2;
		Color32f m_cStartColor1, m_cStartColor2;
		Color32f m_cEndColor1, m_cEndColor2;
		float m_fStartSize1, m_fStartSize2;
		float m_fEndSize1, m_fEndSize2;
		float m_fMass1, m_fMass2;
		Vector3 m_vGravity;
		int m_iParticleCount;
		uint32 m_dwFlags;
		float m_fEmitSpeed;

		IParticleRenderer *m_pRenderer;
		IParticleEmitter *m_pEmitter;

};

}

#endif
