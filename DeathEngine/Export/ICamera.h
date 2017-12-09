
#ifndef _ICAMERA_H_
#define _ICAMERA_H_

#include "IViewFrustrum.h"
#include "../Core/Quaternion.h"

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

class ICamera : public IExportObject
{
	public:

		ICamera() : IExportObject(EOBJ_CAMERA) {}
		virtual ~ICamera() {}

		virtual void Initialize(CameraType _Type,
						const float fZMin,
						const float fZMax,
						const float fXCenter,
						const float fYCenter,
						const int iFOV,
						const float fZoom)=0;

		virtual void SetType(CameraType _Type)=0;
		virtual CameraType GetType() const=0;

		virtual void SetZRange(float fZMin, float fZMax)=0;
		virtual void GetZRange(float &fZMin, float &fZMax)=0;

		virtual void SetZMin(float fZMin)=0;
		virtual float GetZMin() const=0;

		virtual void SetZMax(float fZMax)=0;
		virtual float GetZMax() const=0;

		virtual void SetCenter(float fXCenter, float fYCenter)=0;
		virtual void GetCenter(float &fXCenter, float& fYCenter)=0;

		virtual void SetXCenter(float fXCenter)=0;
		virtual float GetXCenter() const=0;

		virtual void SetYCenter(float fYCenter)=0;
		virtual float GetYCenter() const=0;

		virtual void SetFOV(int iFOV)=0;
		virtual int GetFOV() const=0;

		virtual void SetZoom(float fZoom)=0;
		virtual float GetZoom() const=0;

		virtual void SetPosition(const Core::Vector3& vPos)=0;
		virtual const Core::Vector3& GetPosition() const=0;

		virtual void SetTarget(const Core::Vector3& vPos)=0;
		virtual const Core::Vector3& GetTarget() const=0;

		virtual void SetRotation(const Core::Vector3& vRot)=0;
		virtual const Core::Vector3& GetVRotation() const=0;

		virtual void SetRotation(const Core::Quaternion& qRot)=0;
		virtual const Core::Quaternion& GetQRotation() const=0;

		virtual void GetMatrices(Core::Matrix4& mView, Core::Matrix4& mProj, Core::Matrix4& mViewProj)=0;
		virtual const Core::Matrix4& GetViewMatrix()=0;
		virtual const Core::Matrix4& GetProjectionMatrix()=0;
		virtual const Core::Matrix4& GetViewProjMatrix()=0;

		virtual const IViewFrustrum *GetFrustrum() const=0;

		virtual const Core::Vector3& GetYAxis() const=0;
		virtual const Core::Vector3& GetXAxis() const=0;

		virtual void MakeRayFrom2D(int x, int y, Core::Vector3& vOrigin, Core::Vector3& vDir)=0;
};

#endif