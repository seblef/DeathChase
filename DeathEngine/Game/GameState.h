
#ifndef _GAMESTATE_H_

#define _GAMESTATE_H_

#include "../Core/TSingleton.h"
#include "../Core/ITask.h"
#include "../Core/Handles.h"
#include "GameCamera.h"
#include "PlayerShipController.h"

namespace GUI
{
class IFont;
}

namespace Core
{
class SoundVirtualSource;
}

namespace Game
{

class SpaceShip;

class GameState : public Core::ITask, public Core::TSingleton<GameState>
{
	public:

		GameState(const int iPriority);
		~GameState();

		bool Initialize();
		bool Stop();

		bool OnSuspend();
		bool OnResume();

		bool Update();

		bool Load(const char *szMapName, const char *szMapPath,
			const char *szSpaceShipName, const char *szSpaceShipPath);

	private:

		SpaceShip *m_pShip;

		GameCamera m_Camera;
		PlayerShipController m_Control;

		GUI::IFont *m_pFont;

		uint64 m_dwDeltaTime, m_dwSuspendTime;
		uint32 m_dwFrameCount;

		Core::HSound m_hMusic;
		Core::SoundVirtualSource *m_pMusicSource;
};

}

#define g_Game			Game::GameState::GetSingletonRef()

#endif