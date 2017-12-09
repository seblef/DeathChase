
#ifndef _IOBJECT_H_
#define _IOBJECT_H_

#include "../Core/Transformation.h"
#include "../Core/BSphere.h"
#include "ObjectTypes.h"
#include "IGameEventReceiver.h"
#include <string.h>
#include <stdio.h>

#ifdef DEEXPORT
#include "../Export/IViewFrustrum.h"
#else
#include "../Core/ViewFrustrum.h"
#endif

class CollisionModel3DImpl;

namespace Game
{

class OctreeNode;

class IObject : public IGameEventReceiver
{
	public:

		IObject(ObjectType type) : m_Type(type), m_bVisible(false), m_dwLastFrame(0xffffffff),
			m_bStatic(false), m_bCastShadows(true), m_bReceiveShadows(true),
			m_bLightPassEnable(true), m_bSpecialPassEnable(true),
			m_dwID(0), m_bMustRebuildShadowVolumes(true), m_pColData(0),
			m_pNode(0), m_iNodeIndex(-1), m_bCollideWithWorld(true),
			m_bHasMoved(false)
		{
			m_Transformation.SetBSpheres(&m_BSphere,&m_TransformedBSphere);
		}
		virtual ~IObject() {}

		virtual void Clear()
		{
			RemoveFromOctree();
			m_pColData=0;
		}

		virtual bool Visibility(const IViewFrustrum *pFrustrum)
		{
			m_Transformation.RebuildMatrix();
			m_bVisible=pFrustrum->IsSphereInside(m_TransformedBSphere);
			return m_bVisible;
		}

		bool IsLightPassEnable() const { return m_bLightPassEnable; }
		void SetLightPassEnable(const bool bEnable) { m_bLightPassEnable=bEnable; }

		bool IsSpecialPassEnable() const { return m_bSpecialPassEnable; }
		void SetSpecialPassEnable(const bool bEnable) { m_bSpecialPassEnable=bEnable; }

		bool CastShadows() const { return m_bCastShadows; }
		void SetCastShadows(const bool bCast) { m_bCastShadows=bCast; }

		bool ReceiveShadows() const { return m_bReceiveShadows; }
		void SetReceiveShadows(const bool bReceive) { m_bReceiveShadows=bReceive; }

		bool IsStatic() const { return m_bStatic; }
		bool IsVisible() const { return m_bVisible; }

		virtual bool CastSunShadows(const Core::Vector3& vSunDir) { return true; }
		virtual bool CastPointLightShadows(const Core::Vector3& vLightPos) { return true; }
		virtual bool CastDirLightShadows(const Core::Vector3& vLightDir) { return true; }
		virtual void RebuildShadows() {};

		bool MustRebuildShadowVolumes() const { return m_bMustRebuildShadowVolumes; }
		void SetShadowVolumesRebuilded() { m_bMustRebuildShadowVolumes=false; }

		virtual bool DrawAmbient() { return true; }
		virtual bool DrawSunLight() { return true; }
		virtual bool DrawLight() { return true; }
		virtual bool DrawSpecial() { return true; }
		virtual bool Draw() { return true; }

		virtual bool Update(const float fTime) { return true; }

		Core::Transformation& GetTransformation() { return m_Transformation; }

		const Core::BSphere& GetBSphere() const { return m_BSphere; }
		const Core::BSphere& GetTransformedBSphere() const { return m_TransformedBSphere; }

		ObjectType GetObjectType() const { return m_Type; }

		const char *GetName() const { return m_szName; }
		void SetName(const char *szName) { strcpy(m_szName,szName); }

		uint32 GetID() const { return m_dwID; }
		void SetID(const uint32 dwID) { m_dwID=dwID; }

		void SetLastFrame(const uint32 dwLastFrame) { m_dwLastFrame=dwLastFrame; }
		uint32 GetLastFrame() const { return m_dwLastFrame; }

		virtual bool Parse(FILE *pFile) { return true; }
		virtual bool Save(FILE *pFile) { return true; }

		CollisionModel3DImpl *GetCollisionData() { return m_pColData; }

		void UpdatePositionInOctree();
		void RemoveFromOctree();

		void SetCollideWithWorld(bool bCollide) { m_bCollideWithWorld=bCollide; }
		bool CollideWithWorld() const { return m_bCollideWithWorld; }

		bool HasMoved() const { return m_bHasMoved; }
		void Move() { m_bHasMoved=true; }

	protected:

		Core::Transformation m_Transformation;
		Core::BSphere m_BSphere,m_TransformedBSphere;

		uint32 m_dwLastFrame;
		bool m_bVisible;
		bool m_bCastShadows;
		bool m_bReceiveShadows;
		bool m_bLightPassEnable;
		bool m_bSpecialPassEnable;
		bool m_bStatic;

		bool m_bMustRebuildShadowVolumes;

		CollisionModel3DImpl *m_pColData;

		bool m_bCollideWithWorld;
		bool m_bHasMoved;

	private:

		char m_szName[32];
		ObjectType m_Type;
		uint32 m_dwID;

		friend class OctreeNode;

		void SetNode(OctreeNode *pNode) { m_pNode=pNode; }
		OctreeNode *GetNode() const { return m_pNode; }

		void SetNodeIndex(int iIndex) { m_iNodeIndex=iIndex; }
		int GetNodeIndex() const { return m_iNodeIndex; }

		OctreeNode *m_pNode;
		int m_iNodeIndex;
};

}

#endif
