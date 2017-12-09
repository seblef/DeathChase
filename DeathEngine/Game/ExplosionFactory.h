
#ifndef _EXPLOSIONFACTORY_H_
#define _EXPLOSIONFACTORY_H_

#include "../Core/TSingleton.h"
#include "../Core/Vector.h"
#include <map>
#include <string>

namespace Game
{

class Explosion;
class ExplosionTemplate;

class ExplosionFactory : public Core::TSingleton<ExplosionFactory>
{
	public:

		ExplosionFactory() {}
		~ExplosionFactory() { Release(); }

		void Release();
		void Load();
		Explosion *CreateExplosion(const char *szTemplateName,
			const Core::Vector3& vPos, float fSize,
			bool bSmoke=true, bool bDebris=true, bool bSplash=true);

	protected:

		void ParseFile(const char *szFileName);

		typedef std::map<std::string,ExplosionTemplate*> TemplateMap;
		TemplateMap m_Templates;
};

}

#define g_ExplosionFactory				Game::ExplosionFactory::GetSingletonRef()

#endif