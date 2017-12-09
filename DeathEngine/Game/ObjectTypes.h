
#ifndef _OBJECTTYPES_H_
#define _OBJECTTYPES_H_

enum ObjectType
{
	OBJ_PATCHGRID=0,
	OBJ_WATER,
	OBJ_SHIELD,
	OBJ_MAP,
	OBJ_SPACESHIP,
	OBJ_EDITOR,
	OBJ_MESH,
	OBJ_AMMO,

	OBJ_COUNT

};

const char *const szObjectTypeNames[]=
{
	"PatchGrid",
	"Water",
	"Shield",
	"MapObject",
	"SpaceShipObject",
	"EditorObject",
	"MeshObject",
	"Ammo"

};

#endif