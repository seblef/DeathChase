
#ifndef _MAPOBJECT_H_
#define _MAPOBJECT_H_

#include "TreeDefs.h"
#include "../Core/Handles.h"
#include "../Core/ShadowVolume.h"
#include <stdio.h>

#ifdef DEEXPORT
#include "../Export/IMapObject.h"
#else
#include "IObject.h"

#define IMapObject				Game::MapObject
#endif


namespace Game
{

#ifdef DEEXPORT
class MapObject : public IMapObject
#else
class MapObject : public IObject
#endif
{
	public:

#ifdef DEEXPORT
		MapObject();
		MapObject(int iRadius, float fRadius);
#else
		MapObject();
		MapObject(int iRadius, float fRadius);
#endif
		virtual ~MapObject() {}

		virtual void Clear();
		virtual bool Generate(const char *szName, const char *szBurnedModel);

		virtual bool Visibility(const IViewFrustrum *pFrustrum)
		{
			m_bVisible=pFrustrum->IsSphereInside(m_TransformedBSphere);
			return m_bVisible;
		}

		virtual bool DrawAmbient();
		virtual bool DrawSunLight();
		virtual bool DrawLight() { return true; }
		virtual bool DrawSpecial() { return true; }
		virtual bool Update(const float fTime);

		virtual bool CastSunShadows(const Core::Vector3& vSunDir);
		void RebuildShadows() { m_bMustRebuildShadowVolumes=true; }

		virtual bool Parse(FILE *pFile);
		virtual bool Save(FILE *pFile);

		int GetRadius() const { return m_iRadius; }
		float GetRealRadius() const { return m_fRadius; }

		void SetMapPos(const Core::Vector2i& vMPos) { m_vMapPos=vMPos; }
		const Core::Vector2i& GetMapPos() const { return m_vMapPos; }

		void OnEvent(const GameEvent& Event) {}

		Core::HMesh GetMesh() { return m_bBurned ? m_hBurnedMesh : m_hNormalMesh; }

		float GetHeight() const { return m_fHeight; } // Pour le depot sur le terrain

	protected:

		void InitializeMapObjectInfo(void *pInfo);

		int m_iRadius;
		float m_fRadius;
		Core::Vector2i m_vMapPos;

		Core::HMesh m_hNormalMesh;
		Core::HMesh m_hBurnedMesh;
		TreeType m_Type;
		bool m_bBurned;

		Core::ShadowVolume m_SunShadows;

		char m_szName[64];
		float m_fHeight;
};

}

#endif