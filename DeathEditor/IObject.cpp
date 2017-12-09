
#include "Interfaces.h"
#include "../DeathEngine/Export/IMap.h"
#include "../DeathEngine/Game/IObject.h"

namespace Game
{

void IObject::RemoveFromOctree()
{
	g_Map->RemoveObject(this);
}

void IObject::UpdatePositionInOctree()
{
	g_Map->UpdateObject(this);
}

}