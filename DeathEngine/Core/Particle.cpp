
#include "Particle.h"

namespace Core
{

void Particle::Update(const float fTime)
{
	m_fLife-=fTime;
	if(m_fLife > 0.0f)
	{
		Vector3 vMoment(m_vVelocity * m_fMass);
		m_vPosition+=vMoment * fTime;

		if(m_vPosition.y < 0.0f)
		{
			m_fLife=-1.0f;
			return;
		}

		m_cColor+=m_cColorCounter * fTime;
		m_fSize+=m_fSizeCounter * fTime;
		m_fPercent+=m_fPercentCounter * fTime;

		m_vVelocity*=1.0f - (m_fFriction * fTime);
		m_vVelocity+=m_vGravity * fTime * fTime;
	}
}

}