
#ifndef _PARTICLE_H_
#define _PARTICLE_H_

#include "Vector.h"
#include "Color.h"

namespace Core
{

class ParticleSystem;

class Particle
{
	public:

		Particle() : m_fLife(-1.0f) {}
		~Particle() {}

		void Update(const float fTime);

		float m_fLife,m_fMass,m_fFriction;
		float m_fPercent,m_fPercentCounter;
		Vector3 m_vPosition;
		Vector3 m_vVelocity;
		Vector3 m_vGravity;
		Color32f m_cColor,m_cColorCounter;
		float m_fSize,m_fSizeCounter;

	private:
		friend class ParticleSystem;
		int m_iFactoryIndex;

};

}

#endif