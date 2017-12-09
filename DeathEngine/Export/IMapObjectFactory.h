
#ifndef _IMAPOBJECTFACTORY_H_
#define _IMAPOBJECTFACTORY_H_

#include "../Game/ObjectTypes.h"

namespace Game
{
class IObject;
}

class IMapObjectFactory
{
	public:

		IMapObjectFactory() {}
		virtual ~IMapObjectFactory() {}

		virtual void Clear()=0;

		virtual Game::IObject *CreateObject(ObjectType type)=0;
		virtual bool ReleaseObject(Game::IObject *pObj)=0;

};

#endif
