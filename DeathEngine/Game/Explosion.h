
#ifndef _EXPLOSION_H_
#define _EXPLOSION_H_

#include "../Core/Handles.h"
#include "../Core/Vector.h"
#include "../Core/TMemoryAllocator.h"


namespace Core
{

class Camera;
class ParticleSystem;

}

namespace Game
{

class Explosion
{
	public:

		Explosion() : m_vPosition(Core::Vector3::NullVector), m_fTime(0.0f), m_fLifeTime(1.0f),m_fSize(1.0f),
			m_pSmokeSystem(0), m_pSplashSystem(0), m_pDebrisSystem(0) {}
		~Explosion();

		bool Create(const Core::Vector3& vPos, const float fSize,
			float fLifeTime, Core::HMaterial hMat, Core::HSound hSound,
			bool bSmoke=true, bool bDebris=false, bool bSplash=false,
			const char *szSmokeTemplate=0, const char *szSplashTemplate=0, const char *szDebrisTemplate=0);

		void Update(float fTime);
		void Draw(Core::Camera *pCamera);

		bool IsFinished() const { return m_fTime >= m_fLifeTime; }

		void Clear();
		
		static Explosion *New() { return m_sAllocator.New(); }
		static void Delete(Explosion *pExpl) { m_sAllocator.Delete(pExpl); }
		void Delete() { m_sAllocator.Delete(this); }


	protected:

		Core::Vector3 m_vPosition;
		float m_fTime,m_fLifeTime;
		float m_fSize;
		Core::HMaterial m_hMaterial;
		Core::HSound m_hSound;

		Core::ParticleSystem *m_pSmokeSystem;
		Core::ParticleSystem *m_pSplashSystem;
		Core::ParticleSystem *m_pDebrisSystem;

		static Core::TMemoryAllocator<Explosion> m_sAllocator;
};

}

#endif