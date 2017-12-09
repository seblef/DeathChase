
#ifndef _IMESHMGR_H_
#define _IMESHMGR_H_

#include "../Core/Handles.h"
#include "../Core/Vector.h"

class IMesh;
class ISubMesh;

class IMeshMgr
{
	public:

		IMeshMgr() {}
		virtual ~IMeshMgr() {}

		virtual Core::HMesh LoadMesh(const char *szName, const char *szPath)=0;
		virtual Core::HMesh CreateEmptyMesh(const char *szName)=0;
		virtual void ReleaseMesh(Core::HMesh &hMesh)=0;

		virtual IMesh *GetMesh(Core::HMesh hMesh)=0;
		virtual Core::HMesh GetMeshByName(const char *szName)=0;

		virtual void SetCameraPosition(const Core::Vector3& vPos)=0;
		virtual void DrawMesh(Core::HMesh hMesh,
			const Core::Matrix4& mWorld,
			const Core::Matrix4 *mInverse=0,
			Core::HMaterial hForcedMat=Core::HMaterial(), const bool bImmediate=false)=0;
		virtual void Execute()=0;

		virtual ISubMesh *CreateNewSubMesh()=0;
		virtual void ReleaseSubMesh(ISubMesh *pSubMesh)=0;
};

#endif
