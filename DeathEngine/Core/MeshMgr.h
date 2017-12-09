
#ifndef _MESHMGR_H_
#define _MESHMGR_H_

#include "Mesh.h"
#include "THandleMgr.h"
#include "TSingleton.h"
#include "TMap.h"
#include <map>
#include <string>

#ifdef DEEXPORT
#include "../Export/IMeshMgr.h"
#endif

namespace Core
{

class EdgeSet;

#ifdef DEEXPORT
class MeshMgr : public IMeshMgr, public TSingleton<MeshMgr>
#else
class MeshMgr : public TSingleton<MeshMgr>
#endif
{
	public:

		typedef THandleMgr<Mesh *,HMesh> HMeshMgr;

		MeshMgr(IDirect3DDevice9 *pDevice);
		~MeshMgr();

		HMesh LoadMesh(const char *szName, const char *szPath);
		HMesh CreateEmptyMesh(const char *szName);
		void ReleaseMesh(HMesh &hMesh);

		IMesh *GetMesh(HMesh hMesh);
		EdgeSet *GetMeshEdgeSet(HMesh hMesh);
		Core::HMesh GetMeshByName(const char *szName);

		void SetCameraPosition(const Vector3& vPos) { m_vCamPos=vPos; }
		void DrawMesh(HMesh hMesh,
			const Matrix4& mWorld,
			const Matrix4 *mInverse=0,
			HMaterial hForcedMat=HMaterial(), const bool bImmediate=false);
		void Execute();

#ifdef DEEXPORT
		ISubMesh *CreateNewSubMesh();
		void ReleaseSubMesh(ISubMesh *pSubMesh);
#endif

	protected:

		IDirect3DDevice9 *m_pDevice;

		struct istring_less
		{
			bool operator() (const std::string& l, const std::string& r) const
			{
				return _stricmp(l.c_str(),r.c_str()) < 0;
			}
		};
	
		typedef std::map<std::string,HMesh,istring_less> NameIndex;
		
		HMeshMgr m_Meshes;
		NameIndex m_NameIndex;

		std::vector<Mesh *> m_ExecBuffer;
		Vector3 m_vCamPos;
};

}

#define g_MeshMgr				Core::MeshMgr::GetSingletonRef()

#endif