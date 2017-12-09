
#include "MapObjectFactory.h"
#include "IMapObject.h"
#include "MeshObject.h"

namespace Game
{

MapObjectFactory::MapObjectFactory() :
	m_MapObjAllocator("MapObject"),
	m_MeshAllocator("MeshObject")
{
}

MapObjectFactory::~MapObjectFactory()
{
}

void MapObjectFactory::Clear()
{
	m_MapObjAllocator.ResetAll();
	m_MeshAllocator.ResetAll();
}

IObject *MapObjectFactory::CreateObject(ObjectType type)
{
	switch(type)
	{
		case OBJ_MAP:
			return (IObject *)m_MapObjAllocator.New();

		case OBJ_MESH:
			return (IObject *)m_MeshAllocator.New();

		default:
			return 0;
	}
}

bool MapObjectFactory::ReleaseObject(IObject *pObj)
{
	if(pObj)
	{
		switch(pObj->GetObjectType())
		{
			case OBJ_MAP:
				m_MapObjAllocator.Delete((MapObject *)pObj);
				break;

			case OBJ_MESH:
				m_MeshAllocator.Delete((MeshObject *)pObj);
				break;

			default:
				delete pObj;
				return false;
		}
		return true;
	}
	else
		return false;
}

}