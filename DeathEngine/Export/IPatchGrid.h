
#ifndef _IPATCHGRID_H_
#define _IPATCHGRID_H_

#include "../Core/Handles.h"
#include "../Game/IObject.h"

class IHeightmap;
class IViewFrustrum;

class IPatchGrid : public Game::IObject
{
	public:

		IPatchGrid() : IObject(OBJ_PATCHGRID) {}
		virtual ~IPatchGrid() {}

		virtual void Clear(bool bReleaseTexture=true)=0;
		virtual void Create(IHeightmap *pMap, int iPatchSize)=0;
		virtual void Generate()=0;

		virtual void Reset()=0;

		virtual void Recalculate(int iPosX, int iPosY, int iDist)=0;
		virtual void Simulate(float fTime)=0;

		virtual int GetDrawnPatches()=0;

		virtual void SetVarianceStart(const float fVarStart)=0;
		virtual void SetVarianceRamp(const float fVarRamp)=0;

		virtual void SetTerrainTexture(Core::HTexture hTex)=0;
		virtual Core::HTexture GetTerrainTexture() const=0;

		virtual void SetDetailTexture(Core::HTexture hTex)=0;
		virtual Core::HTexture GetDetailTexture() const=0;
};

#endif