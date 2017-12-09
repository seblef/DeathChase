
#ifndef _ISKYBOX_H_
#define _ISKYBOX_H_

#include "../Core/Handles.h"
#include "../Core/Vector.h"

enum SkyFace
{
	SKY_UP,
	SKY_FRONT,
	SKY_RIGHT,
	SKY_BACK,
	SKY_LEFT,

	SKY_COUNT
};

class ISkyBox
{
	public:

		ISkyBox() {}
		virtual ~ISkyBox() {}

		virtual bool SetTexture(SkyFace iFace, Core::HTexture hTex)=0;
		virtual Core::HTexture GetTexture(SkyFace iFace)=0;
		virtual bool Draw(const Core::Vector3& vCamPos)=0;

};

#endif
