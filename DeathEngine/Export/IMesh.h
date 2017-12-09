
#ifndef _IMESH_H_
#define _IMESH_H_

#include "../Core/Handles.h"
#include "../Core/Vertex.h"
#include "../Core/BBox.h"

struct IDirect3DIndexBuffer9;
struct IDirect3DVertexBuffer9;

class CollisionModel3D;

class ISubMesh
{
	public:

		ISubMesh() {}
		virtual ~ISubMesh() {}

		virtual void Clear()=0;

		virtual bool Construct(int iTriOffset, int iTriCount, Core::HMaterial hMat)=0;
		
		virtual int GetTriOffset() const=0;
		virtual int GetTriCount() const=0;
		virtual Core::HMaterial GetMaterial() const=0;

};

class IMesh
{
	public:

		IMesh() {}
		virtual ~IMesh() {}

		virtual void Clear()=0;

		virtual bool Load(const char *szName, const char *szPath)=0;
		virtual bool Save(const char *szName, const char *szPath)=0;
		virtual bool Construct(const char *szName,
			int iVertCount, Core::VertexPNDTT *pVertices,
			int iTriCount, uint16 *pIndices, ISubMesh *pSubMesh,
			uint32 dwLODDistance)=0;
		virtual bool AddSubMesh(ISubMesh *pSubMesh)=0;
		virtual bool AddLOD(IMesh *pMesh, const uint32 dwDist)=0;
		virtual bool RemoveLOD(const int iIndex)=0;
		virtual IMesh *CreateLOD(uint32 dwDist)=0;

		virtual int GetSubMeshCount()=0;
		virtual ISubMesh *GetSubMesh(int iIndex)=0;
		virtual IDirect3DVertexBuffer9 *GetVertexBuffer() const=0;
		virtual IDirect3DIndexBuffer9 *GetIndexBuffer() const=0;
		virtual const Core::BBox& GetBBox() const=0;
		virtual const char *GetName() const=0;
		virtual const char *GetFileName() const=0;

		virtual uint32 GetMaxLODDistance() const=0;
		virtual void SetMaxLODDistance(const uint32 dwDist)=0;
		virtual int GetLODCount() const=0;
		virtual IMesh *GetLODByDistance(const uint32 dwSqDist)=0;
		virtual IMesh *GetLODByIndex(const int iIndex)=0;

		virtual int GetVerticesCount() const=0;
		virtual int GetTriCount() const=0;

		virtual void Draw(const Core::Matrix4& mWorld, const Core::Matrix4 *pInverse,
			const Core::Vector3& vCamPos, Core::HMaterial hForcedMat=Core::HMaterial())=0;

		virtual CollisionModel3D *GetCollisionData()=0;
};

#endif