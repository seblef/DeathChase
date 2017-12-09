
#ifndef _IPARTICLERENDERER_H_
#define _IPARTICLERENDERER_H_

namespace Core
{

class Particle;
class ScriptFile;
class Camera;

class IParticleRenderer
{
	public:

		IParticleRenderer() {}
		virtual ~IParticleRenderer() {}

		virtual void Draw(Particle *pParticles, int iCount, Camera *pCamera)=0;
		virtual bool Parse(ScriptFile *pFile)=0;

		virtual IParticleRenderer *New()=0;
		virtual void Delete()=0;
};

}

#endif