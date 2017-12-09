
#ifndef _SOUNDLISTENER_H_
#define _SOUNDLISTENER_H_

#include "Vector.h"

namespace Core
{

class SoundListener
{
	public:

		SoundListener() {}
		~SoundListener() {}

		const Vector3& GetPosition();
		void SetPosition(const Vector3& vPos);
		void SetVelocity(const Vector3& vVel);
		void SetOrientation(const Vector3& vZ, const Vector3& vY);
		void SetGain(float fGain);

};

}

#endif
