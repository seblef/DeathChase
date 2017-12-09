
#include "MeshMgr.h"
#include "Renderer.h"

namespace Core
{

MeshMgr::MeshMgr(IDirect3DDevice9 *pDevice) : m_pDevice(pDevice), m_vCamPos(Vector3::NullVector)
{
	SubMesh::InitAllocator();
	Mesh::InitAllocator();
}

MeshMgr::~MeshMgr()
{
	NameIndex::iterator i;
	
	for(i=m_NameIndex.begin();i!=m_NameIndex.end();i++)
	{
		Mesh **pMesh=m_Meshes.Dereference(i->second);
		if(pMesh && *pMesh)
			Mesh::Delete(*pMesh);
	}

	SubMesh::ReleaseAllocator();
	Mesh::ReleaseAllocator();
}

HMesh MeshMgr::LoadMesh(const char *szName, const char *szPath)
{
	if(!szName) return HMesh();

	std::string strName(szName);
	NameIndex::iterator rc=m_NameIndex.find(strName);
	HMesh hMesh;

	if(rc==m_NameIndex.end())
	{
		Mesh **ppMesh=m_Meshes.Acquire(hMesh);
		*ppMesh=Mesh::GetNew();

		if(!(*ppMesh)->Load(szName,szPath))
		{
			m_Meshes.Release(hMesh);
			hMesh=HMesh();
			g_Output->AddMsg("x Erreur [%s]: Impossible de charger le mesh.\n",szName);
		}
		else
			m_NameIndex.insert(std::make_pair(strName,hMesh));
	}
	else
	{
		hMesh=rc->second;
		(*m_Meshes.Dereference(hMesh))->AddRef();
	}

	return hMesh;
}

HMesh MeshMgr::CreateEmptyMesh(const char *szName)
{
	if(!szName) return HMesh();

	std::string strName(szName);
	NameIndex::iterator rc=m_NameIndex.find(strName);
	HMesh hMesh;

	if(rc==m_NameIndex.end())
	{
		Mesh **ppMesh=m_Meshes.Acquire(hMesh);
		*ppMesh=Mesh::GetNew();
		m_NameIndex.insert(std::make_pair(strName,hMesh));
	}
	else
	{
		hMesh=rc->second;
		(*m_Meshes.Dereference(hMesh))->AddRef();
	}

	return hMesh;
}

void MeshMgr::ReleaseMesh(Core::HMesh &hMesh)
{
	Mesh **ppMesh=m_Meshes.Dereference(hMesh);
	if(ppMesh && *ppMesh)
	{
		if((*ppMesh)->RemRef() <= 0)
		{
			if((*ppMesh)->m_bAddedToExecBuffer)
				return;
			else
			{
				NameIndex::iterator i=m_NameIndex.find((*ppMesh)->GetFileName());
				if(i==m_NameIndex.end()) return;

				m_NameIndex.erase(i);

				Mesh::Delete(*ppMesh);
				m_Meshes.Release(hMesh);
			}
		}
	}

	hMesh=HMesh();
}

IMesh *MeshMgr::GetMesh(HMesh hMesh)
{
	Mesh **ppMesh=m_Meshes.Dereference(hMesh);
	if(ppMesh)
		return *ppMesh;
	else
		return 0;
}

EdgeSet *MeshMgr::GetMeshEdgeSet(HMesh hMesh)
{
	Mesh **ppMesh=m_Meshes.Dereference(hMesh);
	if(ppMesh && *ppMesh)
		return (*ppMesh)->GetEdgeSet();
	else
		return 0;
}

Core::HMesh MeshMgr::GetMeshByName(const char *szName)
{
	Core::HMesh hMesh=Core::HMesh();
	if(!szName) return hMesh;

	NameIndex::iterator i=m_NameIndex.find(szName);
	if(i==m_NameIndex.end())
		return hMesh;
	else
		return i->second;
}

void MeshMgr::Execute()
{
	if(m_ExecBuffer.size()==0)
		return;

	g_Renderer.SetVertexDeclaration(VX_PNDTT);

	std::vector<Mesh *>::iterator i;
	for(i=m_ExecBuffer.begin();i!=m_ExecBuffer.end();i++)
		(*i)->Execute();

	m_ExecBuffer.clear();
}

void MeshMgr::DrawMesh(HMesh hMesh,
					   const Matrix4& mWorld,
					   const Matrix4 *pInverse,
					   HMaterial hForcedMat,
					   const bool bImmediate)
{
	Mesh **ppMesh=m_Meshes.Dereference(hMesh);
	if(ppMesh && *ppMesh)
	{
		if(bImmediate)
			(*ppMesh)->Draw(mWorld,pInverse,m_vCamPos,hForcedMat);
		else
		{
			(*ppMesh)->AddInstance(Mesh::MeshInstance(mWorld,pInverse,hForcedMat),m_vCamPos);
			if(!(*ppMesh)->m_bAddedToExecBuffer)
			{
				(*ppMesh)->m_bAddedToExecBuffer=true;
				m_ExecBuffer.push_back(*ppMesh);
			}
		}
	}
}

#ifdef DEEXPORT
ISubMesh *MeshMgr::CreateNewSubMesh()
{
	return  SubMesh::GetNew();
}

void MeshMgr::ReleaseSubMesh(ISubMesh *pSubMesh)
{
	if(pSubMesh)
		SubMesh::Delete((SubMesh *)pSubMesh);
}
#endif

}

