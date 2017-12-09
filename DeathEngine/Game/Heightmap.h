
#ifndef _HEIGHTMAP_H_

#define _HEIGHTMAP_H_

#include "../Common.h"
#include "../Core/Vector.h"
#include "../Core/Handles.h"

#ifdef DEEXPORT
#include "../Export/IHeightmap.h"
#else
#define IHeightmap			Game::Heightmap
#endif

namespace Game
{

#ifdef DEEXPORT
class Heightmap : public IHeightmap
#else
class Heightmap
#endif
{
	public:

		Heightmap() : m_iWidth(0),m_pMap(0),m_fScale(0.0f),m_iWidthPlusOne(1),m_pNormals(0) {};
		~Heightmap();

		bool CreateFlat(const int iSize);
		bool Load(const char *szName, const char *szPathName=0);
		bool Save(const char *szName, const char *szPathName=0);
		void SetMap(int iSize, float *pMap) { m_iWidth=iSize; m_pMap=pMap; }
		
		bool ApplyMask(const int iOffset, const char *szMask, const char *szPathName=0);
		bool Smooth();
		bool Scale(const float fMaxHeight);
		bool Unscale();
		bool BuildNormalMap();

		void Clear();

		int GetSize() const { return m_iWidth; }
		float *GetMap() const { return m_pMap; }
		uint32 *GetNormalMap() const { return m_pNormals; }

		float GetHeight(const int x, const int y) { return m_pMap[x+y*m_iWidthPlusOne]; }
		float GetInterpolatedHeight(float x, float y);

		uint32 GetNormal(const int x, const int y) { return m_pNormals[x+y*(m_iWidthPlusOne)]; }
		void GetNormalByVector(const int x, const int y, Core::Vector3& v);
		void GetInterpolatedNormal(float x, float y, Core::Vector3& v);

		void GetNormalHeight(const int x, const int y, float& fHeight, uint32& dwNorm)
		{
			const int iIndex=x+y*m_iWidthPlusOne;
			fHeight=m_pMap[iIndex];
			dwNorm=m_pNormals[iIndex];
		}
		float GetScale() const { return m_fScale; }

		bool BuildTexture();
		Core::HTexture GetTexture() const { return m_hTexture; }
		bool IsTextureBuilded() const { return !m_hTexture.IsNull(); }

		bool RayIntersect(const Core::Vector3& vOrigin, const Core::Vector3& vDir,
			Core::Vector3& vIntersect);

	protected:

		int m_iWidth;
		int m_iWidthPlusOne;
		float *m_pMap;
		uint32 *m_pNormals;
		float m_fScale;

		Core::HTexture m_hTexture;

};

}

#endif