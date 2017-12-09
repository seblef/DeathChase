
#include "IObject.h"
#include "Octree.h"

namespace Game
{

void IObject::UpdatePositionInOctree()
{
	if(m_pNode) m_pNode->UpdateObject(this);
}

void IObject::RemoveFromOctree()
{
	if(m_pNode) m_pNode->RemoveObject(this);
}

}