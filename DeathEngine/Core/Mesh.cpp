
#include "Mesh.h"
#include "MaterialMgr.h"
#include "MaterialInstance.h"
#include "Renderer.h"
#include "FileSystem.h"
#include "../ColDet/ColDetImpl.h"
#include "../Dirs.h"

namespace Core
{

TMemoryAllocator<SubMesh> *SubMesh::m_sAllocator=0;
TMemoryAllocator<Mesh> *Mesh::m_sAllocator=0;

SubMesh::SubMesh() : m_iTriCount(0),m_iTriOffset(0), m_pMatInstance(0)
{
}

SubMesh::~SubMesh()
{
	Clear();
}

void SubMesh::Clear()
{
	if(m_pMatInstance)
		g_MaterialMgr.ReleaseInstance(m_pMatInstance);

	g_MaterialMgr.ReleaseMaterial(m_hMaterial);

	m_pMatInstance=0;
	m_hMaterial=HMaterial();
	m_iTriOffset=0;
	m_iTriCount=0;
}

bool SubMesh::Construct(int iTriOffset, int iTriCount, HMaterial hMat)
{
	if(hMat.IsNull()) return false;

	Clear();

	if(g_MaterialMgr.IsAnimated(hMat))
		m_pMatInstance=g_MaterialMgr.CreateInstance(hMat);
	
	m_hMaterial=hMat;
	m_iTriOffset=iTriOffset;
	m_iTriCount=iTriCount;

	return true;
}

Mesh::Mesh() : m_dwLODDistance(0),m_pVertexBuffer(0),m_iVerticesCount(0),
	m_pIndexBuffer(0), m_iTriCount(0),
	m_bAddedToExecBuffer(false),
	m_pColData(0)
{
	m_BBox.Init();
	strcpy(m_szName,"mesh");
}

Mesh::~Mesh()
{
	Clear();
}

void Mesh::Clear()
{
	if(m_pColData) delete m_pColData;
	m_pColData=0;

	std::vector<SubMesh*>::iterator i;
	for(i=m_SubMeshes.begin();i!=m_SubMeshes.end();i++)
		SubMesh::Delete(*i);
	m_SubMeshes.clear();

	std::vector<Mesh*>::iterator j;
	for(j=m_LODs.begin();j!=m_LODs.end();j++)
		Mesh::Delete(*j);
	m_LODs.clear();

	if(m_pIndexBuffer) m_pIndexBuffer->Release();
	if(m_pVertexBuffer) m_pVertexBuffer->Release();

	MEMDEBUG_REMEXTRASIZE(m_iVerticesCount * VertexSizeTable[VX_PNDTT]);
	MEMDEBUG_REMEXTRASIZE(m_iTriCount*3*sizeof(uint16));

	m_pIndexBuffer=0;
	m_pVertexBuffer=0;
	m_iTriCount=0;
	m_iVerticesCount=0;

	m_BBox.Init();
	m_dwLODDistance=0;
	strcpy(m_szName,"mesh");
}

bool Mesh::Load(const char *szName, const char *szPath)
{
	if(!szName) return false;

	char szFileName[64];
	char szPathName[512];
//	const int len=strlen(szName);

//	if(len > 4 && strcmp(&szName[len-5],".dgeo")==0)
//	{
		if(szPath)
		{
			strcpy(szFileName,szName);
			strcpy(szPathName,szPath);
		}
		else
		{
			if(!g_FileSystem.ExtractPath(szName,szPathName))
			{
				strcpy(szPathName,DIR_MESHES);
				strcpy(szFileName,szName);
			}
			else
				g_FileSystem.ExtractFileName(szName,szFileName);
		}

		g_FileSystem.ChangeWorkingDirectory(szPathName);
/*	}
	else
	{
		g_FileSystem.ChangeWorkingDirectory(szPath ? szPath : DIR_MESHES);
		strcpy(szTemp,szName);
	}*/

	strcpy(m_szFileName,szName);

	FILE *pFile=fopen(szFileName,"rb");
	if(!pFile)
	{
		g_Output->AddMsg("x Erreur [%s]: Impossible d'ouvrir le fichier de geometrie.\n",szFileName);
		g_FileSystem.ResetWorkingDirectory();
		return false;
	}

	int iLODCount;

	fread(m_szName,64,1,pFile);
	fread(&iLODCount,sizeof(int),1,pFile);

	if(iLODCount < 1)
	{
		g_Output->AddMsg("x Erreur [%s]: Le fichier ne contient aucun LOD.\n",szFileName);
		g_FileSystem.ResetWorkingDirectory();
		fclose(pFile);
		return false;
	}

	if(!LoadFromFile(pFile))
	{
		fclose(pFile);
		Clear();
		g_FileSystem.ResetWorkingDirectory();
		return false;
	}

	for(int i=1;i<iLODCount;i++)
	{
		Mesh *pMesh=Mesh::GetNew();
		if(!pMesh)
		{
			fclose(pFile);
			g_FileSystem.ResetWorkingDirectory();
			return false;
		}

		if(!pMesh->LoadFromFile(pFile))
		{
			g_Output->AddMsg("x Erreur [%s]: Erreur au chargement du LOD %i.\n",szFileName,i);
			g_FileSystem.ResetWorkingDirectory();
			fclose(pFile);
			return false;
		}
		strcpy(pMesh->m_szName,m_szName);
		strcpy(pMesh->m_szFileName,m_szFileName);
		m_LODs.push_back(pMesh);
	}

	fclose(pFile);
	g_FileSystem.ResetWorkingDirectory();

	return true;
}

bool Mesh::Save(const char *szName, const char *szPath)
{
	if(!szName || !m_pVertexBuffer) return false;

	char szTemp[1024];
	g_FileSystem.ChangeWorkingDirectory(szPath ? szPath : DIR_MESHES);

	if(strlen(m_szName)!=0)
	{
		g_FileSystem.CreateDirectory_(m_szName);
		g_FileSystem.ChangeWorkingDirectory(m_szName);
	}

	sprintf(szTemp,"%s.dgeo",szName);

	FILE *pFile=fopen(szTemp,"wb");
	if(!pFile)
	{
		g_Output->AddMsg("x Erreur [%s]: Impossible de creer le fichier pour la sauvegarde.\n");
		return false;
	}

	fwrite(m_szName,64,1,pFile);
	int iLODCount=m_LODs.size() + 1;
	fwrite(&iLODCount,sizeof(int),1,pFile);

	if(!SaveToFile(pFile))
	{
		fclose(pFile);
		g_FileSystem.ResetWorkingDirectory();
		return false;
	}

	std::vector<Mesh*>::iterator i;
	for(i=m_LODs.begin();i!=m_LODs.end();i++)
		if(!(*i)->SaveToFile(pFile))
		{
			fclose(pFile);
			g_FileSystem.ResetWorkingDirectory();
			return false;
		}

	fclose(pFile);
	g_FileSystem.ResetWorkingDirectory();
	return true;
}

bool Mesh::Construct(const char *szName,
					 int iVertCount, VertexPNDTT *pVertices,
					 int iTriCount, uint16 *pIndices, ISubMesh *pSubMesh,
					 uint32 dwLODDistance)
{
	if(!szName || !pVertices || !pSubMesh || !pIndices) return false;

	Clear();

	for(int i=0;i<iVertCount;i++)
		m_BBox.Adjust(pVertices[i].vPos);

	m_EdgeSet.Build(iVertCount,pVertices,iTriCount,pIndices);

	m_pColData=new CollisionModel3DImpl(true);
	CollisionModel3DImpl *pData=(CollisionModel3DImpl *)m_pColData;

	for(int i=0;i<iTriCount;i++)
	{
		pData->addTriangle(*((Vector3D *)&pVertices[pIndices[i*3]].vPos),
			*((Vector3D *)&pVertices[pIndices[i*3+1]].vPos),
			*((Vector3D *)&pVertices[pIndices[i*3+2]].vPos));
	}

	pData->finalize();

	m_pVertexBuffer=g_Renderer.CreateVertexBuffer(pVertices,iVertCount,VX_PNDTT);
	if(!m_pVertexBuffer) return false;
	m_iVerticesCount=iVertCount;

	MEMDEBUG_ADDEXTRASIZE(m_iVerticesCount * VertexSizeTable[VX_PNDTT]);

	m_pIndexBuffer=g_Renderer.CreateIndexBuffer(pIndices,iTriCount*3,D3DFMT_INDEX16);
	if(!m_pIndexBuffer)
	{
		m_pVertexBuffer->Release();
		m_pVertexBuffer=0;
		return false;
	}
	m_iTriCount=iTriCount;

	MEMDEBUG_ADDEXTRASIZE(m_iTriCount*3*sizeof(uint16));

	m_SubMeshes.push_back((Core::SubMesh *)pSubMesh);

	m_dwLODDistance=dwLODDistance;
	strcpy(m_szName,szName);

	return true;
}

bool Mesh::AddSubMesh(ISubMesh *pSubMesh)
{
	if(m_SubMeshes.size()==0 || !pSubMesh ||
		(pSubMesh->GetTriOffset() + pSubMesh->GetTriCount()) > m_iTriCount) return false;

	m_SubMeshes.push_back((Core::SubMesh *)pSubMesh);
	return true;
}

bool Mesh::AddLOD(IMesh *pMesh, const uint32 dwDist)
{
	if(pMesh)
	{
		((Mesh *)pMesh)->m_dwLODDistance=dwDist;
		m_LODs.push_back((Core::Mesh *)pMesh);
		return true;
	}
	else
		return false;
}

IMesh *Mesh::CreateLOD(uint32 dwDist)
{
	Mesh *pMesh=Mesh::GetNew();
	pMesh->m_dwLODDistance=dwDist;
	m_LODs.push_back(pMesh);
	return pMesh;
}

bool Mesh::RemoveLOD(const int iIndex)
{
	if(iIndex >= 1 && iIndex <= m_LODs.size())
	{
		m_sAllocator->Delete(m_LODs[iIndex-1]);
		m_LODs.erase(m_LODs.begin()+(iIndex-1));
		return true;
	}
	else
		return false;
}

ISubMesh *Mesh::GetSubMesh(const int iIndex)
{
	if(iIndex >= 0 && iIndex < m_SubMeshes.size())
		return m_SubMeshes[iIndex];
	else
		return 0;
}

void Mesh::SetMaxLODDistance(const uint32 dwDist)
{
	m_dwLODDistance=dwDist;
}

IMesh *Mesh::GetLODByDistance(const uint32 dwSqDist)
{
	if(dwSqDist==0 || m_LODs.size()==0)
		return this;
	else
	{
		if(dwSqDist < m_dwLODDistance)
			return this;
		else
		{
			std::vector<Mesh*>::iterator i;
			for(i=m_LODs.begin();i!=m_LODs.end();i++)
				if(dwSqDist <= (*i)->GetMaxLODDistance())
					return *i;
		}

		return m_LODs[m_LODs.size()-1];
	}
}

IMesh *Mesh::GetLODByIndex(const int iIndex)
{
	if(iIndex==0) return this;
	else if(iIndex >= 1 && iIndex <= m_LODs.size()) return m_LODs[iIndex-1];
	else return 0;
}

bool Mesh::LoadFromFile(FILE *pFile)
{
	fread(&m_dwLODDistance,sizeof(uint32),1,pFile);

	Vector3 bbox[2];
	fread(bbox,sizeof(Vector3)*2,1,pFile);
	m_BBox.SetMin(bbox[0]);
	m_BBox.SetMax(bbox[1]);

	int iVCount;
	fread(&iVCount,sizeof(int),1,pFile);

	VertexPNDTT *pVert;
	if(iVCount > 0)
	{
		pVert=new VertexPNDTT[iVCount];
		fread(pVert,iVCount * VertexSizeTable[VX_PNDTT],1,pFile);

		m_pVertexBuffer=g_Renderer.CreateVertexBuffer(pVert,iVCount,VX_PNDTT);
		
		if(!m_pVertexBuffer)
		{
			delete[] pVert;
			return false;
		}
	}
	else
		return false;

	m_iVerticesCount=iVCount;
	MEMDEBUG_ADDEXTRASIZE(m_iVerticesCount*VertexSizeTable[VX_PNDTT]);

	int iTriCount;
	fread(&iTriCount,sizeof(int),1,pFile);

	if(iTriCount <= 0)
	{
		delete[] pVert;
		return false;
	}

	uint16 *pInd=new uint16[iTriCount*3];
	fread(pInd,iTriCount * 3 * sizeof(uint16),1,pFile);

	m_pIndexBuffer=g_Renderer.CreateIndexBuffer(pInd,iTriCount*3,D3DFMT_INDEX16);

	m_pColData=new CollisionModel3DImpl(false);
	CollisionModel3DImpl *pData=(CollisionModel3DImpl *)m_pColData;

	for(int i=0;i<iTriCount;i++)
	{
		Vector3D v1,v2,v3;
		v1.x=pVert[pInd[i*3]].vPos.x;
		v1.y=pVert[pInd[i*3]].vPos.y;
		v1.z=pVert[pInd[i*3]].vPos.z;
		v2.x=pVert[pInd[i*3+1]].vPos.x;
		v2.y=pVert[pInd[i*3+1]].vPos.y;
		v2.z=pVert[pInd[i*3+1]].vPos.z;
		v3.x=pVert[pInd[i*3+2]].vPos.x;
		v3.y=pVert[pInd[i*3+2]].vPos.y;
		v3.z=pVert[pInd[i*3+2]].vPos.z;

		pData->addTriangle(v1,v2,v3);
	}

	pData->finalize();

	delete[] pVert;
	delete[] pInd;
	if(!m_pIndexBuffer)
		return false;

	m_iTriCount=iTriCount;
	MEMDEBUG_ADDEXTRASIZE(m_iTriCount * 3 * sizeof(uint16));

	m_EdgeSet.LoadFromFile(pFile);

	int iSubCount;
	fread(&iSubCount,sizeof(int),1,pFile);

	if(iSubCount > 0)
	{
		int tri_offset;
		int tri_count;
		char szMatName[64];
		char szTemp[96];

		for(int i=0;i<iSubCount;i++)
		{
			fread(szMatName,64,1,pFile);
			sprintf(szTemp,"%s.dmat",szMatName);
			HMaterial hMat=g_MaterialMgr.LoadMaterial(szTemp,"");
			
			if(hMat.IsNull())
				return false;

			fread(&tri_offset,sizeof(int),1,pFile);
			fread(&tri_count,sizeof(int),1,pFile);

			SubMesh *pSub=SubMesh::GetNew();
			pSub->Construct(tri_offset,tri_count,hMat);
			m_SubMeshes.push_back(pSub);
		}
	}
	else
		return false;

	return true;
}

bool Mesh::SaveToFile(FILE *pFile)
{
	if(!m_pVertexBuffer || m_SubMeshes.size()==0) return false;

	fwrite(&m_dwLODDistance,sizeof(uint32),1,pFile);

	Vector3 bbox[2]={m_BBox.GetMin(),m_BBox.GetMax()};
	fwrite(bbox,sizeof(Vector3)*2,1,pFile);

	fwrite(&m_iVerticesCount,sizeof(int),1,pFile);

	void *pBuffer;
	if(m_pVertexBuffer->Lock(0,m_iVerticesCount * VertexSizeTable[VX_PNDTT],&pBuffer,0)==D3D_OK)
	{
		fwrite(pBuffer,m_iVerticesCount * VertexSizeTable[VX_PNDTT],1,pFile);
		m_pVertexBuffer->Unlock();
	}
	else
		return false;

	fwrite(&m_iTriCount,sizeof(int),1,pFile);
	if(m_pIndexBuffer->Lock(0,m_iTriCount*3*sizeof(uint16),&pBuffer,0)==D3D_OK)
	{
		fwrite(pBuffer,m_iTriCount*3*sizeof(uint16),1,pFile);
		m_pIndexBuffer->Unlock();
	}
	else
		return false;

	m_EdgeSet.SaveToFile(pFile);

	int iSubCount=m_SubMeshes.size();
	fwrite(&iSubCount,sizeof(int),1,pFile);

	char szMatFileName[1024];

	std::vector<SubMesh*>::iterator i;
	for(i=m_SubMeshes.begin();i!=m_SubMeshes.end();i++)
	{
		IMaterial *pMat=g_MaterialMgr.GetMaterial((*i)->GetMaterial());
		if(pMat)
			fwrite(pMat->GetName(),64,1,pFile);
		else
		{
			char szTemp[64]="";
			fwrite(szTemp,64,1,pFile);
		}

		int tri_offset=(*i)->GetTriOffset();
		int tri_count=(*i)->GetTriCount();
		fwrite(&tri_offset,sizeof(int),1,pFile);
		fwrite(&tri_count,sizeof(int),1,pFile);

		sprintf(szMatFileName,"%s.dmat",pMat->GetName());
		pMat->Save(szMatFileName,"",true);
	}

	return true;
}

void Mesh::Draw(const Matrix4 &mWorld, const Matrix4 *pInverse,
				const Vector3& vCamPos, HMaterial hForcedMat)
{
	if(!m_pVertexBuffer || !m_pIndexBuffer) return;

	g_Renderer.SetVertexDeclaration(VX_PNDTT);
	g_Renderer.SetTransformation(TRANS_MODEL,mWorld,pInverse,pInverse==0);
	g_Renderer.SetVertexBuffer(m_pVertexBuffer,m_iVerticesCount,VertexSizeTable[VX_PNDTT]);
	g_Renderer.SetIndexBuffer(m_pIndexBuffer);

	if(!hForcedMat.IsNull())
	{
		g_MaterialMgr.SetMaterial(hForcedMat);
		g_Renderer.RenderStaticBuffers(D3DPT_TRIANGLELIST,0,m_iTriCount);
	}
	else
	{
		std::vector<SubMesh*>::iterator i;
		for(i=m_SubMeshes.begin();i!=m_SubMeshes.end();i++)
		{
			if((*i)->GetMaterialInstance())
				(*i)->GetMaterialInstance()->Set();
			else
				g_MaterialMgr.SetMaterial((*i)->GetMaterial());

			g_Renderer.RenderStaticBuffers(D3DPT_TRIANGLELIST,(*i)->GetTriOffset()*3,(*i)->GetTriCount());
		}
	}
}

bool Mesh::Execute()
{
	std::vector<Mesh*>::iterator lod;
	for(lod=m_LODs.begin();lod!=m_LODs.end();lod++)
			(*lod)->Execute();

	m_bAddedToExecBuffer=false;
	if(m_Instances.size()==0) return true;

	g_Renderer.SetVertexBuffer(m_pVertexBuffer,m_iVerticesCount,VertexSizeTable[VX_PNDTT]);
	g_Renderer.SetIndexBuffer(m_pIndexBuffer);

	//			TODO: Tester le plus rapide: changer la matric ou le materiau

	if(m_SubMeshes.size()==1)
	{
		if(m_SubMeshes[0]->GetMaterialInstance())
			m_SubMeshes[0]->GetMaterialInstance()->Set();
		else
			g_MaterialMgr.SetMaterial(m_SubMeshes[0]->GetMaterial());

		bool bHasForcedMaterials=false;
		std::vector<MeshInstance>::iterator i;
		for(i=m_Instances.begin();i!=m_Instances.end();i++)
		{
			if(i->hMaterial.IsNull())
			{
				g_Renderer.SetTransformation(TRANS_MODEL,i->mWorld,i->pmInverse,i->pmInverse==0);
				g_Renderer.RenderStaticBuffers(D3DPT_TRIANGLELIST,0,m_iTriCount);
			}
			else
				bHasForcedMaterials=true;
		}

		if(bHasForcedMaterials)
		{
			HMaterial hMat=HMaterial();
			for(i=m_Instances.begin();i!=m_Instances.end();i++)
			{
				if(!i->hMaterial.IsNull())
				{
					if(i->hMaterial!=hMat)
					{
						hMat=i->hMaterial;
						g_MaterialMgr.SetMaterial(hMat);
					}

					g_Renderer.SetTransformation(TRANS_MODEL,i->mWorld,i->pmInverse,i->pmInverse==0);
					g_Renderer.RenderStaticBuffers(D3DPT_TRIANGLELIST,0,m_iTriCount);
				}
			}
		}
	}
	else
	{
		bool bHasForcedMaterials=false;
		std::vector<MeshInstance>::iterator i;
		std::vector<SubMesh*>::iterator s;

		for(s=m_SubMeshes.begin();s!=m_SubMeshes.end();s++)
		{
			const int iOffset=(*s)->GetTriOffset();
			const int iCount=(*s)->GetTriCount();

			if((*s)->GetMaterialInstance())
				(*s)->GetMaterialInstance()->Set();
			else
				g_MaterialMgr.SetMaterial((*s)->GetMaterial());

			for(i=m_Instances.begin();i!=m_Instances.end();i++)
			{
				if(i->hMaterial.IsNull())
				{
					g_Renderer.SetTransformation(TRANS_MODEL,i->mWorld,i->pmInverse,i->pmInverse==0);
					g_Renderer.RenderStaticBuffers(D3DPT_TRIANGLELIST,iOffset*3,iCount);
				}
				else
					bHasForcedMaterials=true;
			}
		}

		if(bHasForcedMaterials)
		{
			HMaterial hMat=HMaterial();
			for(i=m_Instances.begin();i!=m_Instances.end();i++)
			{
				if(!i->hMaterial.IsNull())
				{
					if(i->hMaterial!=hMat)
					{
						hMat=i->hMaterial;
						g_MaterialMgr.SetMaterial(hMat);
					}

					g_Renderer.SetTransformation(TRANS_MODEL,i->mWorld,i->pmInverse,i->pmInverse==0);
					g_Renderer.RenderStaticBuffers(D3DPT_TRIANGLELIST,0,m_iTriCount);
				}
			}
		}
	}

	m_Instances.clear();

	return true;
}

void Mesh::AddInstance(Mesh::MeshInstance &instance, const Vector3& vCamPos)
{
	if(m_LODs.size()==0)
		m_Instances.push_back(instance);
	else
	{
		const Vector3 vDist(instance.mWorld.m[3][0] - vCamPos.x,
							instance.mWorld.m[3][1] - vCamPos.y,
							instance.mWorld.m[3][2] - vCamPos.z);
		const uint32 dwDist=(uint32)vDist.GetSquareLength();

		if(dwDist <= m_dwLODDistance)
			m_Instances.push_back(instance);
		else
		{
			std::vector<Mesh*>::iterator i;
			for(i=m_LODs.begin();i!=m_LODs.end();i++)
				if(dwDist <= (*i)->m_dwLODDistance)
				{
					(*i)->AddInstance(instance,vCamPos);
					return;
				}

			m_LODs[m_LODs.size()-1]->AddInstance(instance,vCamPos);
		}
	}
}

}