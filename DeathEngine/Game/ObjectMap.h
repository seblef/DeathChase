
#ifndef _OBJECTMAP_H_
#define _OBJECTMAP_H_

#include "IMapObject.h"
#include "../Core/TSingleton.h"

#ifdef DEEXPORT
#include "../Export/IObjectMap.h"
#else
#include <vector>

#include "Heightmap.h"
#endif

namespace Game
{

#ifdef DEEXPORT
class ObjectMap : public IObjectMap, public Core::TSingleton<ObjectMap>
#else
class ObjectMap : public Core::TSingleton<ObjectMap>
#endif
{
	public:

		ObjectMap();
		~ObjectMap();

		void Clear();
		void ClearMap();

		bool Generate(IHeightmap *pMap, int iMapRes, float fWaterHeight);
		bool UpdateHeightmap(int x, int y, int iWidth, int iHeight);

		bool AddObject(IMapObject *pObject, bool bOnGround=true);
		bool RemoveObject(IMapObject *pObject);
		std::vector<IMapObject *> *GetObjectsFromPlace(int x, int y, int iRadius=1);

		bool GetFreePlace(int iRadius, int& x, int& y);
		bool IsPlaceUsed(int x, int y, int iRadius=1);
		bool IsUnderWater(int x, int y, int iRadius=1);

	protected:

		class ObjMapEntry
		{
			public:

				ObjMapEntry() {}
				~ObjMapEntry() {}

				float fHeight;
				uint32 dwFlags;
				MapObject *pObject;
		};

		ObjMapEntry *m_pMap;
		IHeightmap *m_pHeightmap;
		int m_iMapSize;

};

}

#define g_ObjectMap				ObjectMap::GetSingletonRef()

#endif