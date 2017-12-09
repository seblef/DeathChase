
#ifndef _IVIEWFRUSTRUM_H_
#define _IVIEWFRUSTRUM_H_

#include "../Core/BBox.h"
#include "../Core/BSphere.h"
#include "IExportObj.h"

class IViewFrustrum : public IExportObject
{
	public:

		IViewFrustrum() : IExportObject(EOBJ_VIEWFRUSTRUM) {}
		virtual ~IViewFrustrum() {}

		virtual void SetPosition(const Core::Vector3& vPos)=0;
		virtual const Core::Vector3& GetPosition() const=0;

		virtual void Transform(const Core::Matrix4& mat)=0;
		virtual Core::Vector3 GetFarLeftUp() const=0;
		virtual Core::Vector3 GetFarLeftDown() const=0;
		virtual Core::Vector3 GetFarRightUp() const=0;
		virtual Core::Vector3 GetFarRightDown() const=0;

		virtual void Create(const Core::Matrix4& mat)=0;

		virtual bool IsPointInside(const Core::Vector3& v) const=0;
		virtual bool IsLineInside(const Core::Vector3& start, const Core::Vector3& end) const=0;
		virtual bool IsBoxInside(const Core::BBox& v) const=0;
		virtual bool IsSphereInside(const Core::BSphere& s) const=0;
};

#endif
