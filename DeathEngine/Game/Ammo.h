
#ifndef _AMMO_H_
#define _AMMO_H_

#include "IObject.h"

namespace Game
{

class WeaponTemplate;

class Ammo : public IObject
{
	public:

		Ammo();
		~Ammo();

		void OnEvent(GameEvent& Event);

		void SetTemplate(WeaponTemplate *pTemplate);
		const WeaponTemplate *GetTemplate() const { return m_pTemplate; }

	protected:

		WeaponTemplate *m_pTemplate;
		float m_fTime;
};

}

#endif