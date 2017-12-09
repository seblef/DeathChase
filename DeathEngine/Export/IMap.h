
#ifndef _IMAP_H_
#define _IMAP_H_

#include <vector>
#include "../Core/Vector.h"

class IHeightmap;
class IPatchGrid;
class IViewFrustrum;
class ISkyBox;
class IWater;
class IShield;
class IObjectMap;
class IMapObjectFactory;
class ITreeGenerator;
class ISun;

namespace Game
{
class IObject;
}

class IMap
{
	public:

		IMap() {}
		virtual ~IMap() {}

		virtual void Release()=0;
		virtual bool Load(const char *szFileName, const char *szPath)=0;
		virtual bool Save()=0;
		virtual bool CreateEmpty(const char *szName, int iSize=256)=0;

		virtual const char *GetName() const=0;

		virtual IHeightmap *GetHeightmap()=0;
		virtual IPatchGrid *GetPatchGrid()=0;
		virtual ISkyBox *GetSkyBox()=0;
		virtual IWater *GetWater()=0;
		virtual IShield *GetShield()=0;
		virtual IObjectMap *GetObjectMap()=0;
		virtual IMapObjectFactory *GetObjectFactory()=0;
		virtual ITreeGenerator *GetTreeGenerator()=0;
		virtual ISun *GetSun()=0;

		virtual void AddObject(Game::IObject *pObj)=0;
		virtual void RemoveObject(Game::IObject *pObj)=0;
		virtual void UpdateObject(Game::IObject *pObj)=0;

		virtual void Draw(const IViewFrustrum *pFrustrum)=0;

		virtual void EnableWater(bool bEnable)=0;
		virtual bool IsWaterEnable() const=0;

		virtual std::vector<Game::IObject *> *Pick(const Core::Vector3& vOrigin, const Core::Vector3& vDir)=0;
		virtual void RebuildAllShadowVolumes()=0;

		virtual std::vector<Game::IObject *> *CollectObjects()=0;
};

#endif