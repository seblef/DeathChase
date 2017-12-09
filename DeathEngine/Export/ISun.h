
#ifndef _ISUN_H_
#define _ISUN_H_

#include "../Core/Vector.h"
#include "../Core/Handles.h"
#include "../Core/Color.h"

class ISun
{
	public:

		ISun() {}
		virtual ~ISun() {}

		virtual void Clear()=0;

		virtual bool Draw()=0;
		virtual bool Generate(Core::HTexture hTex, Core::Color32f cColor, Core::Color32f cAmbientLight,
			Core::Vector3& vDir)=0;

		virtual void SetTexture(Core::HTexture hTex)=0;
		virtual Core::HTexture GetTexture() const=0;

		virtual void SetDirection(const Core::Vector3& vDir)=0;
		virtual const Core::Vector3& GetDirection() const=0;

		virtual void SetColor(const Core::Color32f& cColor)=0;
		virtual const Core::Color32f& GetColor() const=0;

		virtual void SetAmbientLight(const Core::Color32f& cAmbient)=0;
		virtual const Core::Color32f& GetAmbientLight() const=0;

};

#endif
