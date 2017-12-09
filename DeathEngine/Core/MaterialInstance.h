
#ifndef _MATERIALINSTANCE_H_
#define _MATERIALINSTANCE_H_

#include "../Common.h"
#ifdef DEEXPORT
#include "../Export/IMaterial.h"
#else
#define IMaterialInstance		Core::MaterialInstance
#endif

namespace Core
{

class Material;

#ifdef DEEXPORT
class MaterialInstance : public IMaterialInstance
#else
class MaterialInstance
#endif
{
	public:

		MaterialInstance(Material *pMat, uint32 dwAnimTime);
		~MaterialInstance() {}

		void Clear() {}
		void Reset(Material *pMat, uint32 dwAnimTime);
		bool Set();

	protected:

		Material *m_pMaterial;
		uint32 m_dwStartTime;
		uint32 m_dwAnimTime;

};

}

#endif