
#include "SoundListener.h"
#include "../../SDK/OpenAL/al.h"

namespace Core
{

const Vector3& SoundListener::GetPosition()
{
	static Vector3 vPos;
	alGetListenerfv(AL_POSITION,(float *)&vPos);

	return vPos;
}

void SoundListener::SetPosition(const Vector3& vPos)
{
	alListenerfv(AL_POSITION,(const float *)&vPos);
}

void SoundListener::SetVelocity(const Vector3& vVel)
{
	alListenerfv(AL_VELOCITY,(const float *)&vVel);
}

void SoundListener::SetOrientation(const Vector3& vZ, const Vector3& vY)
{
	float fOrientation[6]={vZ.x,vZ.y,vZ.z,vY.x,vY.y,vY.z};
	alListenerfv(AL_ORIENTATION,fOrientation);
}

void SoundListener::SetGain(float fGain)
{
	alListenerf(AL_GAIN,fGain);
}

}