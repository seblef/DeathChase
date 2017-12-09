
#ifndef _GAMETEST_H_
#define _GAMETEST_H_

#ifdef DEEXPORT

#include "Game/SpaceShip.h"
#include "Game/GameCamera.h"
#include "Game/PlayerShipController.h"

#include <windows.h>

namespace GUI
{

class IFont;

}

class GameTest
{
	public:

		GameTest(Game::SpaceShip *pShip, HWND hWnd);
		~GameTest() { Clear(); }

		void Clear();

		bool Initialize();
		bool Stop();

		bool Update(const uint64 dwTime);

	protected:

		Game::SpaceShip *m_pShip;
		Game::GameCamera m_Camera;
		Game::PlayerShipController m_PlayerControl;

		GUI::IFont *m_pFont;

		HWND m_hWnd;
};

#endif

#endif