
#ifndef _MAPOBJECTFACTORY_H_
#define _MAPOBJECTFACTORY_H_

#include "../Core/TSingleton.h"
#include "../Core/TMemoryAllocator.h"
#include "IObject.h"

#ifdef DEEXPORT
#include "../Export/IMapObjectFactory.h"
#endif

namespace Game
{

class MeshObject;
class MapObject;

#ifdef DEEXPORT
class MapObjectFactory : public IMapObjectFactory, public Core::TSingleton<MapObjectFactory>
#else
class MapObjectFactory : public Core::TSingleton<MapObjectFactory>
#endif
{
	public:

		MapObjectFactory();
		~MapObjectFactory();

		void Clear();

		IObject *CreateObject(ObjectType type);
		bool ReleaseObject(IObject *pObj);

	protected:

		Core::TMemoryAllocator<MapObject> m_MapObjAllocator;
		Core::TMemoryAllocator<MeshObject> m_MeshAllocator;

};

}

#define g_MapObjectFactory				Game::MapObjectFactory::GetSingletonRef()

#endif