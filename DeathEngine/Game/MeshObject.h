
#ifndef _MESHOBJECT_H_
#define _MESHOBJECT_H_

#include "IObject.h"
#include "../Core/ShadowVolume.h"

#ifdef DEEXPORT
#include "../Export/IMeshObject.h"
#else
#include "../Core/Handles.h"
#endif

namespace Game
{

#ifdef DEEXPORT
	class MeshObject : public IMeshObject
#else
class MeshObject : public IObject
#endif
{
	public:

		MeshObject();
		~MeshObject() { Clear(); }

		void Clear();

		void SetMesh(Core::HMesh hMesh);
		Core::HMesh GetMesh() const { return m_hMesh; }

		bool Visibility(const IViewFrustrum *pFrustrum)
		{
			m_bVisible=pFrustrum->IsSphereInside(m_TransformedBSphere);
			return m_bVisible;
		}

		bool DrawAmbient();
		bool DrawSunLight();
		bool DrawLight() { return true; }
		bool DrawSpecial() { return true; }
		bool Update(const float fTime);

		bool CastSunShadows(const Core::Vector3& vSunDir);
		void RebuildShadows() { m_bMustRebuildSunShadows=true; }

		bool Parse(FILE *pFile);
		bool Save(FILE *pFile);

		void OnEvent(const GameEvent& Event);

		void SetAnimation(const int iAxe, const float fSpeed);
		void GetAnimation(int& iAxe, float& fSpeed) const { iAxe=m_iAxe; fSpeed=m_fSpeed; }
		bool IsAnimated() const { return m_bAnim; }
		void DisableAnimation() { m_bAnim=false; m_bStatic=true; }

	protected:

		void InitializeMeshInfo(void *pInfo);

		Core::HMesh m_hMesh;
		Core::ShadowVolume m_SunShadows;

		bool m_bMustRebuildSunShadows;
		float m_fTime;

		bool m_bAnim;
		float m_fSpeed;
		float m_fAngle;
		int m_iAxe;
};

}

#endif