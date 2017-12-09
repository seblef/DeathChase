
#ifndef _IPARTICLEEMITTER_H_
#define _IPARTICLEEMITTER_H_

#include "Vector.h"

namespace Core
{

class Particle;
class ScriptFile;

class IParticleEmitter
{
	public:

		IParticleEmitter() {}
		virtual ~IParticleEmitter() {}

		virtual void Emit(const Core::Vector3& vPos, Particle *pParticle)=0;
		virtual void Emit(const Core::Vector3& vPos, Particle *pParticles, int iCount)=0;

		virtual bool Parse(ScriptFile *pFile)=0;

		virtual IParticleEmitter *New()=0;
		virtual void Delete()=0;

};

}

#endif