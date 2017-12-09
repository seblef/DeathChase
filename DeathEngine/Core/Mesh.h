
#ifndef _MESH_H_
#define _MESH_H_

#include "TMemoryAllocator.h"
#include "IResource.h"
#include "../Dev/MemDebugger.h"
#include "MaterialInstance.h"
#include "EdgeSet.h"

#ifdef DEEXPORT
#include "../Export/IMesh.h"
#else
#include "Vertex.h"
#include "BBox.h"
#include "Handles.h"

#define ISubMesh			Core::SubMesh
#define IMesh				Core::Mesh

struct IDirect3DIndexBuffer9;
struct IDirect3DVertexBuffer9;

#endif

struct IDirect3DDevice9;
class CollisionModel3D;

namespace Core
{

#ifdef DEEXPORT
class SubMesh : public ISubMesh
#else
class SubMesh
#endif
{
	public:

		SubMesh();
		~SubMesh();

		void Clear();

		bool Construct(int iTriOffset, int iTriCount, HMaterial hMat);
		
		int GetTriCount() const { return m_iTriCount; }
		int GetTriOffset() const { return m_iTriOffset; }
		HMaterial GetMaterial() const { return m_hMaterial; }
		IMaterialInstance *GetMaterialInstance() const { return m_pMatInstance; }

		MEMDEBUG_IMPL
		MEMDEBUG_CLASSNAME("SubMesh")
		MEMDEBUG_CLASSSIZE(sizeof(SubMesh))

		static void InitAllocator() { m_sAllocator=new TMemoryAllocator<SubMesh>("SubMesh"); }
		static void ReleaseAllocator() { if(m_sAllocator) delete m_sAllocator; m_sAllocator=0; }

		static SubMesh *GetNew() { return m_sAllocator->New(); }
		static void Delete(SubMesh *pSubMesh) { m_sAllocator->Delete(pSubMesh); }

	protected:

		int m_iTriCount;
		int m_iTriOffset;
		HMaterial m_hMaterial;
		IMaterialInstance *m_pMatInstance;

		static TMemoryAllocator<SubMesh> *m_sAllocator;
};


#ifdef DEEXPORT
class Mesh : public IResource, public IMesh
#else
class Mesh : public IResource
#endif
{
	public:

		Mesh();
		~Mesh();

		void Clear();

		bool Load(const char *szName, const char *szPath);
		bool Save(const char *szName, const char *szPath);
		bool Construct(const char *szName,
			int iVertCount, VertexPNDTT *pVertices,
			int iTriCount, uint16 *pInd, ISubMesh *pSubMesh,
			uint32 dwLODDistance);
		bool AddSubMesh(ISubMesh* pSubMesh);
		bool AddLOD(IMesh *pMesh, const uint32 dwDist);
		bool RemoveLOD(const int iIndex);
		IMesh *CreateLOD(uint32 dwDist);

		int GetSubMeshCount() { return m_SubMeshes.size(); }
		ISubMesh *GetSubMesh(int iIndex);

		IDirect3DVertexBuffer9 *GetVertexBuffer() const { return m_pVertexBuffer; }
		IDirect3DIndexBuffer9 *GetIndexBuffer() const { return m_pIndexBuffer; }
		EdgeSet *GetEdgeSet() { return m_LODs.size() > 0 ? m_LODs[m_LODs.size()-1]->GetEdgeSet() : &m_EdgeSet; }

		const BBox& GetBBox() const { return m_BBox; }
		const char *GetName() const { return m_szName; }
		const char *GetFileName() const { return m_szFileName; }

		uint32 GetMaxLODDistance() const { return m_dwLODDistance; }
		void SetMaxLODDistance(const uint32 dwDist);
		int GetLODCount() const { return m_LODs.size()+1; }
		IMesh *GetLODByDistance(const uint32 dwSqDist);
		IMesh *GetLODByIndex(const int iIndex);

		int GetVerticesCount() const { return m_iVerticesCount; }
		int GetTriCount() const { return m_iTriCount; }

		void Draw(const Matrix4& mWorld, const Matrix4 *pInverse,
			const Vector3& vCamPos, HMaterial hForcedMat=HMaterial());

		CollisionModel3D *GetCollisionData() { return m_pColData; }

		MEMDEBUG_IMPL
		MEMDEBUG_CLASSNAME("Mesh")
		MEMDEBUG_CLASSSIZE(sizeof(Mesh))

		static void InitAllocator() { m_sAllocator=new TMemoryAllocator<Mesh>("Mesh"); }
		static void ReleaseAllocator() { if(m_sAllocator) delete m_sAllocator; m_sAllocator=0; }
		static Mesh *GetNew() { return m_sAllocator->New(); }
		static void Delete(Mesh *pMesh) { m_sAllocator->Delete(pMesh); }

		//			Pour le buffer d'execution du mesh manager

		class MeshInstance
		{
			public:

				MeshInstance(const Matrix4& _mWorld, const Matrix4 *pInverse,
					const HMaterial _hMat) : mWorld(_mWorld),pmInverse(pInverse), hMaterial(_hMat) {}
				MeshInstance(const MeshInstance& inst) : mWorld(inst.mWorld), pmInverse(inst.pmInverse), hMaterial(inst.hMaterial) {}
				~MeshInstance() {}

				Matrix4 mWorld;
				HMaterial hMaterial;
				const Matrix4 *pmInverse;
		};

		void AddInstance(MeshInstance& instance, const Vector3& vCamPos);
		bool m_bAddedToExecBuffer;

		bool Execute();

		//			Pour les lods
		bool LoadFromFile(FILE *pFile);
		bool SaveToFile(FILE *pFile);

	protected:

		uint32 m_dwLODDistance;
		std::vector<Mesh*> m_LODs;

		std::vector<SubMesh*> m_SubMeshes;
		IDirect3DVertexBuffer9 *m_pVertexBuffer;
		IDirect3DIndexBuffer9 *m_pIndexBuffer;
		int m_iVerticesCount;
		int m_iTriCount;
		BBox m_BBox;

		EdgeSet m_EdgeSet;
		CollisionModel3D *m_pColData;

		char m_szName[64];
		char m_szFileName[128];

		std::vector<MeshInstance> m_Instances;

		static TMemoryAllocator<Mesh> *m_sAllocator;
};

}

#endif