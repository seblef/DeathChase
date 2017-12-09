
#ifndef _VIEWFRUSTRUM_H_

#define _VIEWFRUSTRUM_H_

#include "Plane.h"

#ifdef DEEXPORT
#include "../Export/IViewFrustrum.h"
#else
#define IViewFrustrum		Core::ViewFrustrum
#endif

namespace Core
{

#ifdef DEEXPORT
class ViewFrustrum : public IViewFrustrum
#else
class ViewFrustrum
#endif
{
	public:

		typedef enum _ViewPlane
		{
			VF_FAR_PLANE=0,
			VF_NEAR_PLANE=1,
			VF_LEFT_PLANE=2,
			VF_RIGHT_PLANE=3,
			VF_BOTTOM_PLANE=4,
			VF_TOP_PLANE=5,

			VF_PLANE_COUNT
		} ViewPlane;

		ViewFrustrum() {};
		ViewFrustrum(const Matrix4& mat);

		void Clear() {}

		void SetPosition(const Vector3& vPos) { m_vPosition=vPos; };
		const Vector3& GetPosition() const { return m_vPosition; };

		void Transform(const Matrix4& mat);
		Vector3 GetFarLeftUp() const;
		Vector3 GetFarLeftDown() const;
		Vector3 GetFarRightUp() const;
		Vector3 GetFarRightDown() const;

		void Create(const Matrix4& mat);

		bool IsPointInside(const Vector3& v) const;
		bool IsLineInside(const Vector3& start, const Vector3& end) const;
		bool IsBoxInside(const BBox& v) const;
		bool IsSphereInside(const BSphere& s) const;

	protected:

		Vector3 m_vPosition;
		Plane m_Planes[6];
};

}

#endif