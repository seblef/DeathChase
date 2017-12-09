
#ifndef _IHEIGHTMAP_H_
#define _IHEIGHTMAP_H_

#include "../Core/Vector.h"

class IHeightmap
{
	public:

		IHeightmap() {}
		virtual ~IHeightmap() {}

		virtual bool CreateFlat(const int iSize)=0;
		virtual bool Load(const char *szName, const char *szPathName=0)=0;
		
		virtual bool ApplyMask(const int iOffset, const char *szMask, const char *szPathName=0)=0;
		virtual bool Smooth()=0;
		virtual bool Scale(const float fMaxHeight)=0;
		virtual bool Unscale()=0;
		virtual bool BuildNormalMap()=0;

		virtual void Clear()=0;

		virtual int GetSize() const=0;
		virtual float *GetMap() const=0;
		virtual float GetHeight(const int x, const int y)=0;
		virtual float GetScale() const=0;
		virtual uint32 GetNormal(const int x, const int y)=0;

		virtual bool BuildTexture()=0;
		virtual bool IsTextureBuilded() const=0;

		virtual bool RayIntersect(const Core::Vector3& vOrigin, const Core::Vector3& vDir,
			Core::Vector3& vIntersect)=0;

		virtual float GetInterpolatedHeight(float x, float y)=0;
		virtual void GetNormalByVector(const int x, const int y, Core::Vector3& v)=0;
		virtual void GetInterpolatedNormal(float x, float y, Core::Vector3& v)=0;

};

#endif
