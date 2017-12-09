
#ifndef _PARTICLESYSTEM_H_
#define _PARTICLESYSTEM_H_

#include "IParticleRenderer.h"
#include "IParticleEmitter.h"
#include "ParticleSystemTemplate.h"
#include "TMemoryAllocator.h"
#include "BBox.h"
#include <vector>

namespace Core
{

class Camera;

class ParticleSystem
{
	public:

		ParticleSystem();
		~ParticleSystem();

		void Clear();

		bool Create(const ParticleSystemTemplate& t);
		bool Create(int iParticleCount,
			float fLife1, float fLife2,
			float fFriction1, float fFriction2,
			const Vector3& vVelocity1, const Vector3& vVelocity2,
			const Color32f& cStartColor1, const Color32f& cStartColor2,
			const Color32f& cEndColor1, const Color32f& cEndColor2,
			float fStartSize1, float fStartSize2,
			float fEndSize1, float fEndSize2,
			float fMass1, float fMass2,
			const Vector3& vGravity,
			IParticleRenderer *pRenderer,
			IParticleEmitter *pEmitter,
			bool bEmitOnce=false,
			bool bEmitLoop=false,
			float fEmitSpeed=0.0f);

		void Update(const float fTime);
		void Draw(Camera *pCamera);

		bool IsFinished() const { return m_bFinished; }

		void SetPosition(const Vector3& vPos) { m_vPosition=vPos; }
		const Vector3& GetPosition() const { return m_vPosition; }

		const BBox& GetBBox() const { return m_BBox; }

		void SetRenderer(IParticleRenderer *pRenderer) { m_Template.m_pRenderer=pRenderer; }
		void SetEmitter(IParticleEmitter *pEmitter) { m_Template.m_pEmitter=pEmitter; }

		void StopEmission() { m_bStopEmission=true; }

		static ParticleSystem *New() { return m_sAllocator.New(); }
		void Delete() { m_sAllocator.Delete(this); }

	protected:

		void CreateNewParticle(Particle *pPart);

		ParticleSystemTemplate m_Template;

		std::vector<int> m_FreeIndex;
		Particle *m_pParticles;
		int m_iParticlesCount;

		float m_fTime;
		float m_fBBoxTime;

		bool m_bStopEmission;
		bool m_bFinished;
		Vector3 m_vPosition;

		BBox m_BBox;

		static TMemoryAllocator<ParticleSystem> m_sAllocator;
};

}

#endif