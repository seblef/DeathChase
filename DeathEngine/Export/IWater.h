
#ifndef _IWATER_H_
#define _IWATER_H_

#include "../Core/Handles.h"
#include "../Game/IObject.h"

class IViewFrustrum;
class IHeightmap;

class IWater : public Game::IObject
{
	public:

		IWater() : IObject(OBJ_WATER) {}
		virtual ~IWater() {}

		virtual void Clear()=0;

		virtual bool Generate(IHeightmap *pHMap,
			const int iRes,
			const float fHeight,
			const bool bSurround,
			const bool bDynamic=true)=0;
		
		virtual void Simulate(const uint32 dwTime)=0;

		virtual void SetWaterTexture(Core::HTexture hTex)=0;
		virtual Core::HTexture GetWaterTexture() const=0;

		virtual bool IsSurroundEnable() const=0;
		virtual float GetHeight() const=0;
};

#endif