
#ifndef _PARTICLEENGINE_H_
#define _PARTICLEENGINE_H_

#include "TSingleton.h"
#include "TMemoryAllocator.h"
#include <map>
#include <string>

namespace Core
{

class ParticleSystemTemplate;
class ParticleSystem;
class IParticleEmitter;
class IParticleRenderer;
class Camera;

class ParticleEngine : public TSingleton<ParticleEngine>
{
	public:

		ParticleEngine();
		~ParticleEngine();

		void Clear();
		void Release();

		bool Load();

		ParticleSystem *CreateSystem(const char *szTemplateName=0);

		void PlaySystem(ParticleSystem *pSystem, bool bAutoDelete);
		void StopSystem(ParticleSystem *pSystem);

		void Update(float fTime);
		void Draw(Camera *pCamera);

		IParticleEmitter *CreateEmitter(const char *szName);
		IParticleRenderer *CreateRenderer(const char *szName);

	protected:

		void ParseFile(const char *szFileName);

		TMemoryAllocator<ParticleSystem> m_Allocator;

		class SystemItem
		{
			public:

				SystemItem() : pSystem(0),bAutoDelete(false) {}
				SystemItem(ParticleSystem *pSys, bool bDelete) : pSystem(pSys), bAutoDelete(bDelete) {}
				SystemItem(const SystemItem& item) : pSystem(item.pSystem), bAutoDelete(item.bAutoDelete) {}
				~SystemItem() {}

				const SystemItem& operator=(const SystemItem& item)
				{
					pSystem=item.pSystem;
					bAutoDelete=item.bAutoDelete;
					return *this;
				}

				ParticleSystem *pSystem;
				bool bAutoDelete;
		};

		std::vector<SystemItem> m_Systems;
		std::map<std::string,ParticleSystemTemplate *> m_Templates;

};

}

#define g_ParticleEngine			Core::ParticleEngine::GetSingletonRef()

#endif