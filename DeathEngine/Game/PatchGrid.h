
#ifndef _PATCHGRID_H_

#define _PATCHGRID_H_

#include "HeightmapTexCoords.h"
#include "../Core/ViewFrustrum.h"
#include "../Core/Handles.h"
#include "IObject.h"

#ifdef DEEXPORT
#include "../Export/IPatchGrid.h"
#else
#define IPatchGrid				Game::PatchGrid
#endif

namespace Game
{

#define PATCHSHADER_NOTEXTURE				0
#define PATCHSHADER_AMBIENT_TEX				1
#define PATCHSHADER_AMBIENT_TEXDETAIL		2
#define PATCHSHADER_SUN_TEX					3
#define PATCHSHADER_SUN_TEXDETAIL			4

#define PATCHSHADER_COUNT					5

class Patch;

#ifdef DEEXPORT
class PatchGrid : public IPatchGrid
#else
class PatchGrid : public IObject
#endif
{
	public:

		PatchGrid();
		~PatchGrid();

		void Clear(bool bReleaseTexture=true);
		void Create(IHeightmap *pMap, int iPatchSize);
		void Generate();

		void Reset();

		void Recalculate(int iPosX, int iPosY, int iDist);
		void Simulate(float fTime);

		void OnEvent(const GameEvent& Event) {}

		bool Visibility(const IViewFrustrum *pFrustrum);
		bool DrawAmbient();
		bool DrawSunLight();

		int GetDrawnPatches() { return m_iDrawnPatches; };

		void SetVarianceStart(const float fVarStart) { m_fVarianceStart=fVarStart; };
		void SetVarianceRamp(const float fVarRamp) { m_fVarianceRamp=fVarRamp; };

		void SetTerrainTexture(Core::HTexture hTex) { m_hTerrainTexture=hTex; }
		Core::HTexture GetTerrainTexture() const { return m_hTerrainTexture; }

		void SetDetailTexture(Core::HTexture hTex) { m_hDetailTexture=hTex; }
		Core::HTexture GetDetailTexture() const { return m_hDetailTexture; }

	protected:

		void Tesselate(const Core::Vector3& vPos);

		Core::Vector3 m_vLastPos;
		int m_iDrawnPatches;
		int m_iWidth,m_iPatchSize;
		float m_fTime;
		Patch **m_ppPatches;
		Heightmap *m_pMap;
		HeightmapTexCoords m_TexCoords;

		float m_fVarianceRamp,m_fVarianceStart;

		Core::HTexture m_hTerrainTexture;
		Core::HTexture m_hDetailTexture;
		Core::HTexture m_hHeightmapTexture;

		Core::HShader m_hShaders[PATCHSHADER_COUNT];
};

}

#endif