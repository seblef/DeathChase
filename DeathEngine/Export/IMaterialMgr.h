
#ifndef _IMATERIALMGR_H_
#define _IMATERIALMGR_H_

#include "IMaterial.h"

class IMaterialMgr
{
	public:

		IMaterialMgr() {}
		virtual ~IMaterialMgr() {}

		virtual Core::HMaterial LoadMaterial(const char *szName, const char *szPath=0)=0;
		virtual Core::HMaterial CreateEmptyMaterial(const char *szName)=0;
		virtual void ReleaseMaterial(Core::HMaterial &hMat)=0;

		virtual bool SetMaterial(Core::HMaterial hMat)=0;

		virtual bool IsAnimated(Core::HMaterial hMat)=0;
		virtual IMaterialInstance *CreateInstance(Core::HMaterial hMat)=0;
		virtual void ReleaseInstance(IMaterialInstance *pInst)=0;

		virtual IMaterial *GetMaterial(Core::HMaterial hMat)=0;
		virtual Core::HMaterial GetMaterialByName(const char *szName)=0;

		virtual IMaterial *CreateMaterialWithoutRegistering()=0;
		virtual void ReleaseMaterialWithoutRegistering(IMaterial *pMat)=0;

		virtual Core::HMaterial RegisterMaterial(IMaterial *pMat)=0;

		virtual void SetRenderingPass(int iRenderingPass)=0;
};

#endif
