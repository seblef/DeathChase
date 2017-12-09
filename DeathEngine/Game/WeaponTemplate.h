
#ifndef _WEAPONTEMPLATE_H_
#define _WEAPONTEMPLATE_H_

#include "../Core/Handles.h"

namespace Game
{

class IWeaponTrajectory;

enum WeaponType
{
	WT_LASER,
	WT_ROCKET,
	
	WT_COUNT

};

enum WeaponAmmoType
{
	WAT_LINE,
	WAT_SPRITE,
	WAT_MESH,

	WAT_COUNT
};

class WeaponTemplate
{
	public:

		WeaponTemplate() : m_iCount(0), m_Type(WT_LASER), m_AmmoType(WAT_LINE),
			m_fSize(1.0f), m_fDeltaFireTime(0.1f), m_fDamage(20.0f), m_pTrajectory(0) {}
		~WeaponTemplate() {}

		int m_iCount;
		WeaponType m_Type;
		WeaponAmmoType m_AmmoType;

		Core::HMesh m_hMesh;
		Core::HMaterial m_hMaterial;
		Core::HTexture m_hIcon;

		float m_fSize;
		float m_fDeltaFireTime;
		float m_fDamage;

		char m_szName[32];

		IWeaponTrajectory *m_pTrajectory;

		bool m_bAmbientPass;
		bool m_bSunPass;
		bool m_bSpecialPass;
};

}

#endif