
#ifndef _WATER_H_
#define _WATER_H_

#include "Heightmap.h"
#include "WaterSurround.h"
#include "../Core/ViewFrustrum.h"
#include "IObject.h"

struct IDirect3DVertexBuffer9;
struct IDirect3DIndexBuffer9;

#ifdef DEEXPORT
#include "../Export/IWater.h"
#else
#define IWater				Game::Water
#endif

namespace Game
{

#ifdef DEEXPORT
class Water : public IWater
#else
class Water : public IObject
#endif
{
	public:

		Water();
		~Water();

		void Clear();

		bool Generate(IHeightmap *pHMap,
			const int iRes,
			const float fHeight,
			const bool bSurround,
			const bool bDynamic=true);
		
		void Simulate(const uint32 dwTime);
		void OnEvent(const GameEvent& Event) {}

		bool Visibility(const IViewFrustrum *pFrustrum);
		bool Draw();

		void SetWaterTexture(Core::HTexture hTex);
		Core::HTexture GetWaterTexture() const { return m_hTexture; }

		bool IsSurroundEnable() const { return m_bSurround; }
		float GetHeight() const { return m_fHeight; }

	protected:

		class WaterEntry
		{
			public:

				float fHeight;
				float fForce;
				float fVelocity;
				float fDepth;
				bool *pbVisible;
				Core::Vector3 vNormal;
		};

		class WaterPatch
		{
			public:

				WaterPatch();
				~WaterPatch();

				void Clear();
				bool Generate(int iStartX, int iStartY,
					int iX, int iY,
					int iWidth, int iHeight,
					int iMapWidth, int iMapRes,
					WaterEntry *pMap, IHeightmap *pHMap);
				void Update(WaterEntry *pMap, int iMapWidth);

				Core::BBox m_BBox;
				bool m_bVisible;

				IDirect3DVertexBuffer9 *m_pVBuf;
				IDirect3DIndexBuffer9 *m_pIBuf;

				int m_iVertCount;
				int m_iTriCount;

				int m_iStartX,m_iStartY;
				int m_iWidth,m_iHeight;
		};

		Core::HTexture m_hTexture;
		Core::HShader m_hStaticShader;
		Core::HShader m_hDynamicShader;

		int m_iResolution;
		float m_fHeight;
		IHeightmap *m_pHMap;

		WaterEntry *m_pMap;
		WaterPatch *m_pPatches;
		int m_iPatchesCount;

		bool m_bDynamic;
		bool m_bSurround;
		WaterSurround m_Surround;
};

}

#endif