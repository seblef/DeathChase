
#ifndef _PARTICLEEMITTERSTD_H_
#define _PARTICLEEMITTERSTD_H_

#include "IParticleEmitter.h"
#include "TMemoryAllocator.h"

namespace Core
{

class ParticleEngine;

class ParticleEmitter_Linear : public IParticleEmitter
{
	public:

		ParticleEmitter_Linear() : m_vPos1(Core::Vector3::NullVector), m_vPos2(Core::Vector3::NullVector) {}
		~ParticleEmitter_Linear() {}

		void Emit(const Core::Vector3& vPos, Particle *pParticle);
		void Emit(const Core::Vector3& vPos, Particle *pParticles, int iCount);

		bool Parse(ScriptFile *pFile) { return true; }

		void SetPositionRange(const Core::Vector3& v1, const Core::Vector3& v2) { m_vPos1=v1; m_vPos2=v2; }
		void GetPositionRange(Core::Vector3& v1, Core::Vector3& v2) { v1=m_vPos1; v2=m_vPos2; }

		void Clear() {}

		IParticleEmitter *New() { return m_sAllocator.New(); }
		void Delete() { m_sAllocator.Delete(this); }

	protected:

		Core::Vector3 m_vPos1,m_vPos2;

		friend class ParticleEngine;
		static TMemoryAllocator<ParticleEmitter_Linear> m_sAllocator;

};

class ParticleEmitter_ExplosionRing : public IParticleEmitter
{
	public:

		ParticleEmitter_ExplosionRing() {}
		~ParticleEmitter_ExplosionRing() {}

		void Emit(const Core::Vector3& vPos, Particle *pParticle);
		void Emit(const Core::Vector3& vPos, Particle *pParticles, int iCount);

		bool Parse(ScriptFile *pFile) { return true; }

		void Clear() {}

		IParticleEmitter *New() { return m_sAllocator.New(); }
		void Delete() { m_sAllocator.Delete(this); }

	protected:

		friend class ParticleEngine;
		static TMemoryAllocator<ParticleEmitter_ExplosionRing> m_sAllocator;

};

class ParticleEmitter_Debris : public IParticleEmitter
{
	public:

		ParticleEmitter_Debris() {}
		~ParticleEmitter_Debris() {}

		void Emit(const Core::Vector3& vPos, Particle *pParticle);
		void Emit(const Core::Vector3& vPos, Particle *pParticles, int iCount);

		bool Parse(ScriptFile *pFile) { return true; }

		void Clear() {}

		IParticleEmitter *New() { return m_sAllocator.New(); }
		void Delete() { m_sAllocator.Delete(this); }

	protected:

		friend class ParticleEngine;
		static TMemoryAllocator<ParticleEmitter_Debris> m_sAllocator;
};

class ParticleEmitter_Spray : public IParticleEmitter
{
	public:

		ParticleEmitter_Spray()  : m_fWidth(1.0f) {}
		~ParticleEmitter_Spray() {}

		void Emit(const Core::Vector3& vPos, Particle *pParticle);
		void Emit(const Core::Vector3& vPos, Particle *pParticles, int iCount);

		bool Parse(ScriptFile *pFile) { return true; }

		void SetWidth(const float fWidth) { m_fWidth=fWidth; }
		float GetWidth() const { return m_fWidth; }

		void Clear() {}

		IParticleEmitter *New() { return m_sAllocator.New(); }
		void Delete() { m_sAllocator.Delete(this); }

	protected:

		float m_fWidth;

		friend class ParticleEngine;
		static TMemoryAllocator<ParticleEmitter_Spray> m_sAllocator;
};

class ParticleEmitter_Smoke : public IParticleEmitter
{
	public:

		ParticleEmitter_Smoke() {}
		~ParticleEmitter_Smoke() {}

		void Emit(const Core::Vector3& vPos, Particle *pParticle);
		void Emit(const Core::Vector3& vPos, Particle *pParticles, int iCount);

		bool Parse(ScriptFile *pFile) { return true; }

		void Clear() {}

		IParticleEmitter *New() { return m_sAllocator.New(); }
		void Delete() { m_sAllocator.Delete(this); }

	protected:

		friend class ParticleEngine;
		static TMemoryAllocator<ParticleEmitter_Smoke> m_sAllocator;
};

class ParticleEmitter_Explosion : public IParticleEmitter
{
	public:

		ParticleEmitter_Explosion() {}
		~ParticleEmitter_Explosion() {}

		void Emit(const Core::Vector3& vPos, Particle *pParticle);
		void Emit(const Core::Vector3& vPos, Particle *pParticles, int iCount);

		bool Parse(ScriptFile *pFile) { return true; }

		void Clear() {}

		IParticleEmitter *New() { return m_sAllocator.New(); }
		void Delete() { m_sAllocator.Delete(this); }

	protected:

		friend class ParticleEngine;
		static TMemoryAllocator<ParticleEmitter_Explosion> m_sAllocator;

};

class ParticleEmitter_Precipitation : public IParticleEmitter
{
	public:

		ParticleEmitter_Precipitation() {}
		~ParticleEmitter_Precipitation() {}

		void Emit(const Core::Vector3& vPos, Particle *pParticle);
		void Emit(const Core::Vector3& vPos, Particle *pParticles, int iCount);

		bool Parse(ScriptFile *pFile) { return true; }

		void Clear() {}

		IParticleEmitter *New() { return m_sAllocator.New(); }
		void Delete() { m_sAllocator.Delete(this); }

	protected:

		friend class ParticleEngine;
		static TMemoryAllocator<ParticleEmitter_Precipitation> m_sAllocator;
};

}

#endif