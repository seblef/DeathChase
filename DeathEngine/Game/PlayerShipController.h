
#ifndef _PLAYERSHIPCONTROLLER_H_

#define _PLAYERSHIPCONTROLLER_H_

#include "ISpaceShipController.h"

namespace Game
{

class PlayerShipController : public ISpaceShipController
{
	public:

		PlayerShipController();
		~PlayerShipController();

		void Initialize();
		void ProcessKeys(char *pKeys, char *pLastKeys);

};

}

#endif