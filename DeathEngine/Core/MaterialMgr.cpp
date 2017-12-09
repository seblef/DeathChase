
#include "MaterialMgr.h"

namespace Core
{

MaterialMgr::MaterialMgr(IDirect3DDevice9 *pDevice) : m_pDevice(pDevice), m_InstAlloc("MaterialInstance")
{
	MYASSERT(pDevice,"D3D Device invalide",true);
}

MaterialMgr::~MaterialMgr()
{
	NameIndex::iterator i;
	
	for(i=m_NameIndex.begin();i!=m_NameIndex.end();i++)
	{
		Material *pMat=*m_Materials.Dereference(i->second);

		if(pMat)
		{
			pMat->Release();
			delete pMat;
		}
	}
}

HMaterial MaterialMgr::LoadMaterial(const char *szName, const char *szPath)
{
	if(!szName) return HMaterial();

	std::string strName(szName);
	HMaterial hMat;
	NameIndex::iterator rc=m_NameIndex.find(strName);

	if(rc==m_NameIndex.end())
	{
		Material **pMat=m_Materials.Acquire(hMat);
		*pMat=new Material(m_pDevice);
		
		if(!(*pMat)->Load(szName,szPath))
		{
			delete *pMat;
			m_Materials.Release(hMat);
			hMat=HMaterial();
			g_Output->AddMsg("x Erreur lors du chargement du material %s\n",szName);
		}
		else
		{
			(*pMat)->SetName(strName.c_str());
			m_NameIndex.insert(std::make_pair(strName,hMat));
		}
	}
	else
	{
		hMat=rc->second;
		Material *pMat=*m_Materials.Dereference(hMat);
		pMat->AddRef();
	}
	
	return hMat;
}

HMaterial MaterialMgr::CreateEmptyMaterial(const char *szName)
{
	if(!szName) return HMaterial();

	HMaterial hMat;
	std::string strName(szName);
	NameIndex::iterator rc=m_NameIndex.find(strName);

	if(rc==m_NameIndex.end())
	{
		Material **ppMat=m_Materials.Acquire(hMat);
		*ppMat=new Material(m_pDevice);
		(*ppMat)->Create(szName);
		m_NameIndex.insert(std::make_pair(strName,hMat));
	}
	else
	{
		hMat=rc->second;
		(*m_Materials.Dereference(hMat))->AddRef();
	}

	return hMat;
}

void MaterialMgr::ReleaseMaterial(HMaterial &hMat)
{
	Material **pMat=m_Materials.Dereference(hMat);
	
	if(pMat && *pMat)
	{
		if((*pMat)->RemRef() <= 0)
		{
			NameIndex::iterator i=m_NameIndex.find((*pMat)->GetName());
			if(i!=m_NameIndex.end())
				m_NameIndex.erase(i);

			(*pMat)->Release();
			delete *pMat;
			m_Materials.Release(hMat);
		}
	}

	hMat=HMaterial();
}

bool MaterialMgr::SetMaterial(HMaterial hMat)
{
	Material **ppMat=m_Materials.Dereference(hMat);

	if(ppMat && *ppMat)
	{
		if((*ppMat)->Set(0))
			return true;
	}

	return false;
}

IMaterial *MaterialMgr::GetMaterial(HMaterial hMat)
{
	Material **ppMat=m_Materials.Dereference(hMat);
	if(ppMat)
		return (IMaterial *)*ppMat;
	else
		return 0;
}

HMaterial MaterialMgr::GetMaterialByName(const char *szName)
{
	HMaterial hMat=HMaterial();
	if(!szName) return hMat;

	NameIndex::iterator i=m_NameIndex.find(szName);
	if(i==m_NameIndex.end())
		return hMat;
	else
		return i->second;
}

bool MaterialMgr::IsAnimated(Core::HMaterial hMat)
{
	Material **ppMat=m_Materials.Dereference(hMat);
	if(ppMat && *ppMat)
		return (*ppMat)->IsAnimated();
	else
		return false;
}

IMaterialInstance *MaterialMgr::CreateInstance(Core::HMaterial hMat)
{
	Material **ppMat=m_Materials.Dereference(hMat);
	if(ppMat && *ppMat && (*ppMat)->IsAnimated())
	{
		MaterialInstance *pInst=m_InstAlloc.GetFree();
		if(pInst)
			pInst->Reset(*ppMat,(*ppMat)->GetAnimTime());
		else
		{
			pInst=new MaterialInstance(*ppMat,(*ppMat)->GetAnimTime());
			m_InstAlloc.RegisterNew(pInst);
		}

		return pInst;
	}
	else
		return 0;
}

void MaterialMgr::ReleaseInstance(IMaterialInstance *pInst)
{
	if(pInst)
		m_InstAlloc.Delete((MaterialInstance *)pInst);
}

IMaterial *MaterialMgr::CreateMaterialWithoutRegistering()
{
	return new Material(m_pDevice);
}

void MaterialMgr::ReleaseMaterialWithoutRegistering(IMaterial *pMat)
{
	if(pMat) delete pMat;
}

HMaterial MaterialMgr::RegisterMaterial(IMaterial *pMat)
{
	HMaterial hMat=HMaterial();

	if(!pMat) return hMat;

	std::string strName(pMat->GetName());

	NameIndex::iterator i=m_NameIndex.find(strName);
	if(i==m_NameIndex.end())
	{
		Material **ppMat=m_Materials.Acquire(hMat);
		*ppMat=(Material *)pMat;
		m_NameIndex.insert(std::make_pair(strName,hMat));
	}
	else
		hMat=i->second;

	return hMat;
}

void MaterialMgr::SetRenderingPass(int iRenderingPass)
{
	Material::SetRenderingPass(iRenderingPass);
}

}