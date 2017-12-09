
#ifndef _ISPACESHIP_H_
#define _ISPACESHIP_H_

#include "../Core/Vector.h"
#include "../Core/Handles.h"
#include "../Game/IObject.h"
#include "IExportObj.h"


class ISpaceShip : public IExportObject, public Game::IObject
{
	public:

		ISpaceShip() : IExportObject(EOBJ_SPACESHIP), IObject(OBJ_SPACESHIP) {}
		virtual ~ISpaceShip() {}

		virtual bool Load(const char *szFileName, const char *szPath)=0;
		virtual bool Save(const char *szFileName, const char *szPath)=0;

		virtual void AddMainFirePoint(const Core::Vector3& vFire)=0;
		virtual void AddSecFirePoint(const Core::Vector3& vFire)=0;
		virtual void AddBackFirePoint(const Core::Vector3& vFire)=0;
		virtual void AddDefensePoint(const Core::Vector3& vDefense)=0;
		virtual void AddBombDropPoint(const Core::Vector3& vDrop)=0;

		virtual int GetMainFirePointCount() const=0;
		virtual int GetSecFirePointCount() const=0;
		virtual int GetBackFirePointCount() const=0;
		virtual int GetDefensePointCount() const=0;
		virtual int GetBombDropPointCount() const=0;

		virtual Core::Vector3 GetMainFirePoint(const int iIndex)=0;
		virtual Core::Vector3 GetSecFirePoint(const int iIndex)=0;
		virtual Core::Vector3 GetBackFirePoint(const int iIndex)=0;
		virtual Core::Vector3 GetDefensePoint(const int iIndex)=0;
		virtual Core::Vector3 GetBombDropPoint(const int iIndex)=0;

		virtual void SetMesh(Core::HMesh hMesh)=0;
		virtual Core::HMesh GetMesh() const=0;

		virtual void SetLinearSpeed(const float fSpeed)=0;
		virtual float GetLinearSpeed() const=0;

		virtual void SetRotationSpeed(const float fSpeed)=0;
		virtual float GetRotationSpeed() const=0;

};

#endif