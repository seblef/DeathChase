
#ifndef _IOBJECTMAP_H_
#define _IOBJECTMAP_H_

#include <vector>


class IMapObject;
class IHeightmap;
class IViewFrustrum;

class IObjectMap
{
	public:

		IObjectMap() {}
		virtual ~IObjectMap() {}

		virtual void Clear()=0;

		virtual void ClearMap()=0;

		virtual bool Generate(IHeightmap *pMap, int iMapRes, float fWaterHeight)=0;
		virtual bool UpdateHeightmap(int x, int y, int iWidth, int iHeight)=0;

		virtual bool AddObject(IMapObject *pObject, bool bOnGround=true)=0;
		virtual bool RemoveObject(IMapObject *pObject)=0;
		virtual std::vector<IMapObject *> *GetObjectsFromPlace(int x, int y, int iRadius=1)=0;

		virtual bool GetFreePlace(int iRadius, int& x, int& y)=0;
		virtual bool IsPlaceUsed(int x, int y, int iRadius=1)=0;
		virtual bool IsUnderWater(int x, int y, int iRadius=1)=0;

};

#endif
