
#ifndef _SOBJECT_H_
#define _SOBJECT_H_

#include "../DeathEngine/Core/Handles.h"
#include "../DeathEngine/Core/Transformation.h"
#include "../DeathEngine/Core/BSphere.h"
#include <string.h>

enum SObjectType
{
	SOBJ_SPACESHIP=0,
	SOBJ_MAINFIRE,
	SOBJ_SECFIRE,
	SOBJ_BOMBDROP,
	SOBJ_BACKFIRE,
	SOBJ_DEFENSE,

	SOBJ_COUNT

};

const char * const SObjectNames[]=
{
	"SpaceShipObj",
	"MainFireObj",
	"SecFireObj",
	"BombDropObj",
	"BackFireObj",
	"DefenseObj"

};

class CollisionModel3D;

class SObject
{
	public:

		SObject(SObjectType _Type);
		~SObject();

		void Clear();
		void Draw();
		void Transform();

		SObjectType GetType() const { return m_Type; }

		Core::Transformation& GetTransformation() { return m_Transformation; }
		Core::BSphere& GetOriginalBSphere() { return m_OriginalBSphere; }
		Core::BSphere& GetBSphere() { return m_TransBSphere; }

		Core::HMesh GetMesh() const { return m_hMesh; }
		void SetMesh(Core::HMesh hMesh);

		void SetName(const char *szName) { strcpy(m_szName,szName); }
		const char *GetName() const { return m_szName; }

		void ConstructMesh();

		CollisionModel3D *GetColData() { return m_pColData; }

	protected:

		Core::HMesh m_hMesh;
		Core::Transformation m_Transformation;
		Core::BSphere m_OriginalBSphere;
		Core::BSphere m_TransBSphere;

		static Core::HMesh m_shTemplates[SOBJ_COUNT-1];
		bool m_bTemplate;

		SObjectType m_Type;
		char m_szName[64];

		CollisionModel3D *m_pColData;
};

#endif