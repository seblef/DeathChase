
#include "PlayerShipController.h"
#define DIRECTINPUT_VERSION 0x0700
#include <dinput.h>

#include "../MyAssert.h"

namespace Game
{

PlayerShipController::PlayerShipController() {}
PlayerShipController::~PlayerShipController() {}

void PlayerShipController::Initialize()
{
	m_vRotation=Core::Vector3::NullVector;
}

void PlayerShipController::ProcessKeys(char *pKeys, char *pLastKeys)
{
	MYASSERT(pKeys && pLastKeys,"Bad key pointer",true);

	m_vRotation=Core::Vector3::NullVector;

	if(KEYDOWN(pKeys[DIK_DOWN]))
		m_vRotation.x=1.0f;
	else if(KEYDOWN(pKeys[DIK_UP]))
		m_vRotation.x=-1.0f;

	if(KEYDOWN(pKeys[DIK_LEFT]))
		m_vRotation.z=-2.0f;
	else if(KEYDOWN(pKeys[DIK_RIGHT]))
		m_vRotation.z=2.0f;

	m_bStartFire=false;
	m_bEndFire=false;

	if(KEYDOWN(pKeys[DIK_SPACE]) && (!(KEYDOWN(pLastKeys[DIK_SPACE]))))
		m_bStartFire=true;
	else if(KEYDOWN(pLastKeys[DIK_SPACE]) && (!(KEYDOWN(pKeys[DIK_SPACE]))))
		m_bEndFire=true;
}

}