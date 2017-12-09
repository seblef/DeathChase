
#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "ViewFrustrum.h"
#include "Quaternion.h"

#ifdef DEEXPORT
#include "../Export/ICamera.h"
#else

enum CameraType
{
	CAM_PERSPECTIVE=0,
	CAM_FRONT,
	CAM_BACK,
	CAM_RIGHT,
	CAM_LEFT,
	CAM_TOP,
	CAM_BOTTOM,

	CAM_COUNT

};

#define ICamera				Core::Camera

#endif

namespace Core
{

#ifdef DEEXPORT
class Camera : public ICamera
#else
class Camera
#endif
{
	public:

		Camera();
		Camera(const Camera& cam);
		~Camera() {}

		void Clear();

		void Initialize(CameraType _Type,
						const float fZMin,
						const float fZMax,
						const float fXCenter,
						const float fYCenter,
						const int iFOV,
						const float fZoom);

		void SetType(CameraType _Type);
		CameraType GetType() const { return m_Type; }

		void SetZRange(float fZMin, float fZMax);
		void GetZRange(float &fZMin, float &fZMax) { fZMin=m_fZMin; fZMax=m_fZMax; }

		void SetZMin(float fZMin);
		float GetZMin() const { return m_fZMin; }

		void SetZMax(float fZMax);
		float GetZMax() const { return m_fZMax; }

		void SetCenter(float fXCenter, float fYCenter) { m_fXCenter=fXCenter; m_fYCenter=fYCenter; m_bHasChanged=true; m_bProjHasChanged=true; }
		void GetCenter(float &fXCenter, float& fYCenter) { fXCenter=m_fXCenter; fYCenter=m_fYCenter; }

		void SetXCenter(float fXCenter) { m_fXCenter=fXCenter; m_bHasChanged=true; m_bProjHasChanged=true; }
		float GetXCenter() const { return m_fXCenter; }

		void SetYCenter(float fYCenter) { m_fYCenter=fYCenter; m_bHasChanged=true; m_bProjHasChanged=true; }
		float GetYCenter() const { return m_fYCenter; }

		void SetFOV(int iFOV);
		int GetFOV() const { return m_iFOV; }

		void SetZoom(float fZoom);
		float GetZoom() const { return m_fZoom; }

		void SetPosition(const Vector3& vPos);
		const Vector3& GetPosition() const { return m_vPosition; }

		void SetTarget(const Vector3& vPos);
		const Vector3& GetTarget() const { return m_vTarget; }

		void SetRotation(const Vector3& vRot);
		const Vector3& GetVRotation() const { return m_vRotation; }

		void SetRotation(const Quaternion& qRot);
		const Quaternion& GetQRotation() const { return m_qRotation; }

		void GetMatrices(Matrix4& mView, Matrix4& mProj, Matrix4& mViewProj);
		const Matrix4& GetViewMatrix();
		const Matrix4& GetProjectionMatrix();
		const Matrix4& GetViewProjMatrix();

		const IViewFrustrum *GetFrustrum() const { return (IViewFrustrum *)&m_Frustrum; }

		const Vector3& GetYAxis() const { return m_vYAxis; }
		const Vector3& GetXAxis() const { return m_vXAxis; }

		void MakeRayFrom2D(int x, int y, Vector3& vOrigin, Vector3& vDir);

	protected:

		void RebuildMatrices();
		virtual void RebuildViewMatrix();

		bool m_bHasChanged;
		bool m_bProjHasChanged;
		bool m_bTargetEnable;

		CameraType m_Type;
		float m_fZMin,m_fZMax;
		float m_fXCenter,m_fYCenter;
		float m_fZoom;
		int m_iFOV;

		Vector3 m_vPosition;
		Vector3 m_vTarget;
		Vector3 m_vRotation;
		Quaternion m_qRotation;
		
		Vector3 m_vXAxis;
		Vector3 m_vYAxis;

		Matrix4 m_mProjection;
		Matrix4 m_mView;
		Matrix4 m_mViewProj;
		Matrix4 m_mInvView;

		ViewFrustrum m_Frustrum;
};

}

#endif