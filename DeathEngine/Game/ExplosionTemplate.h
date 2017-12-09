
#ifndef _EXPLOSIONTEMPLATE_H_
#define _EXPLOSIONTEMPLATE_H_

#include "../Core/Handles.h"

namespace Game
{

class Explosion;

class ExplosionTemplate
{
	public:

		ExplosionTemplate() : m_iCount(1), m_fLifeTime1(1.0f),m_fLifeTime2(2.0f),
			m_bSmoke(true), m_bSplash(false), m_bDebris(false) {}
		~ExplosionTemplate() {}

		float m_fLifeTime1, m_fLifeTime2;
		Core::HSound m_hSound;
		Core::HMaterial m_hMaterial;
		int m_iCount;

		bool m_bDebris;
		bool m_bSplash;
		bool m_bSmoke;

		char m_szDebrisSystemName[32];
		char m_szSmokeSystemName[32];
		char m_szSplashSystemName[32];
};

}

#endif