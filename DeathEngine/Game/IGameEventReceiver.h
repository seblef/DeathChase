
#ifndef _IGAMEEVENTRECEIVER_H_
#define _IGAMEEVENTRECEIVER_H_

#include "GameEvent.h"

namespace Game
{

class IGameEventReceiver
{
	public:

		IGameEventReceiver() {}
		virtual ~IGameEventReceiver() {}

		virtual void OnEvent(const GameEvent& Event)=0;

};

}

#endif