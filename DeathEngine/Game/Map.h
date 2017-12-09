
#ifndef _MAP_H_
#define _MAP_H_

#include "../Core/TSingleton.h"
#include "Heightmap.h"
#include "PatchGrid.h"
#include "SkyBox.h"
#include "Water.h"
#include "Shield.h"
#include "Sun.h"
#include "../Core/ViewFrustrum.h"

#ifdef DEEXPORT
#include "../Export/IMap.h"
#else
#include <vector>
#endif

namespace Game
{

#ifdef DEEXPORT
class Map : public IMap, public Core::TSingleton<Map>
#else
class Map : public Core::TSingleton<Map>
#endif
{
	public:

		Map();
		~Map();

		void Release();
		bool Load(const char *szFileName, const char *szPath);
		bool Save();
		bool CreateEmpty(const char *szName, int iSize=256);

		const char *GetName() const { return m_szName; }

		IHeightmap *GetHeightmap() { return (IHeightmap *)m_pHeightmap; }
		IPatchGrid *GetPatchGrid() { return (IPatchGrid *)m_pPatchGrid; }
		ISkyBox *GetSkyBox() { return (ISkyBox *)m_pSkyBox; }
		IWater *GetWater() { return (IWater *)m_pWater; }
		IShield *GetShield() { return (IShield *)m_pShield; }
		ISun *GetSun() { return (ISun *)m_pSun; }

#ifdef DEEXPORT
		IObjectMap *GetObjectMap();
		IMapObjectFactory *GetObjectFactory();
		ITreeGenerator *GetTreeGenerator();

		void AddObject(IObject *pObject);
		void RemoveObject(IObject *pObject);
		void UpdateObject(IObject *pObject);

		std::vector<IObject *> *CollectObjects();
#endif

		void EnableWater(bool bEnable) { m_bWaterEnable=bEnable; }
		bool IsWaterEnable() const { return m_bWaterEnable; }

		void Visibility(const IViewFrustrum *pFrustrum);
		void Draw(const IViewFrustrum *pFrustrum);

		void Simulate(const float fTime);

		std::vector<IObject *> * Pick(const Core::Vector3& vOrigin, const Core::Vector3& vDir);
		void RebuildAllShadowVolumes();

	protected:

		void InitializeMapInfo(void *pInfo);

		Heightmap *m_pHeightmap;
		PatchGrid *m_pPatchGrid;
		SkyBox *m_pSkyBox;
		Water *m_pWater;
		Shield *m_pShield;
		Sun *m_pSun;

		bool m_bWaterEnable;

		char m_szName[64];

};

}

#define g_Map				Game::Map::GetSingletonRef()

#endif