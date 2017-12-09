
#ifndef _MATERIALMGR_H_
#define _MATERIALMGR_H_

#include "MaterialInstance.h"
#include "Material.h"
#include "TSingleton.h"
#include "TMemoryAllocator.h"
#include "THandleMgr.h"

#ifdef DEEXPORT
#include "../Export/IMaterialMgr.h"
#endif

namespace Core
{

#ifdef DEEXPORT
class MaterialMgr : public IMaterialMgr, public TSingleton<MaterialMgr>
#else
class MaterialMgr : public TSingleton<MaterialMgr>
#endif
{
	public:

		typedef THandleMgr<Material *,HMaterial> HMaterialMgr;

		MaterialMgr(IDirect3DDevice9 *pDevice);
		~MaterialMgr();

		HMaterial LoadMaterial(const char *szName, const char *szPath=0);
		HMaterial CreateEmptyMaterial(const char *szName);
		void ReleaseMaterial(HMaterial &hMat);

		bool SetMaterial(HMaterial hMat);

		bool IsAnimated(HMaterial hMat);
		IMaterialInstance *CreateInstance(HMaterial hMat);
		void ReleaseInstance(IMaterialInstance *pInst);

		IMaterial *GetMaterial(HMaterial hMat);
		Core::HMaterial GetMaterialByName(const char *szName);

		IMaterial *CreateMaterialWithoutRegistering();
		void ReleaseMaterialWithoutRegistering(IMaterial *pMat);

		HMaterial RegisterMaterial(IMaterial *pMat);

		void SetRenderingPass(int iRenderingPass);

	protected:

		IDirect3DDevice9 *m_pDevice;

		struct istring_less
		{
			bool operator() (const std::string& l, const std::string& r) const
			{
				return _stricmp(l.c_str(),r.c_str()) < 0;
			}
		};
	
		typedef std::map<std::string,HMaterial,istring_less> NameIndex;
		
		HMaterialMgr m_Materials;
		NameIndex m_NameIndex;

		TMemoryAllocator<MaterialInstance> m_InstAlloc;

};

}

#define g_MaterialMgr				Core::MaterialMgr::GetSingletonRef()

#endif