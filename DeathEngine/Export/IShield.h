
#ifndef _ISHIELD_H_
#define _ISHIELD_H_

#include "../Core/Handles.h"
#include "../Core/Vector.h"


class IShield
{
	public:

		IShield() {}
		virtual ~IShield() {}

		virtual bool Create(const Core::Vector3& vSize, int iMapSize,
			uint32 dwVisDist, Core::HTexture hTex)=0;
		virtual void Clear()=0;

		virtual Core::HTexture GetTexture() const=0;
		virtual void SetTexture(Core::HTexture hTex)=0;

		virtual Core::HShader GetShader() const=0;
		virtual void SetShader(Core::HShader hShader)=0;

		virtual uint32 GetVisibilityDistance() const=0;
		virtual void SetVisibilityDistance(uint32 dwDist)=0;

		virtual const Core::Vector3& GetPosition() const=0;
		virtual const Core::Vector3& GetSize() const=0;
		virtual void SetSize(const Core::Vector3& vSize)=0;

		virtual bool Draw()=0;
		virtual void SetCameraPosition(const Core::Vector3& vCamPos)=0;

		virtual void EnableShield(const bool bEnable)=0;

};

#endif