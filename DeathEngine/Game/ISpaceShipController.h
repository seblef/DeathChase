
#ifndef _ISPACESHIPCONTROLLER_H_
#define _ISPACESHIPCONTROLLER_H_

#include "../Core/Vector.h"

namespace Game
{

class ISpaceShipController
{
	public:

		ISpaceShipController() : m_bStartFire(false), m_bEndFire(false) {};
		~ISpaceShipController() {};

		virtual void Initialize()=0;

		const Core::Vector3& GetRotation() const { return m_vRotation; };
		
		const bool StartFire() const { return m_bStartFire; };
		const bool EndFire() const { return m_bEndFire; };


	protected:

		Core::Vector3 m_vRotation;
		bool m_bStartFire,m_bEndFire;

};

}

#endif