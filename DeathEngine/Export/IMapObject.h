
#ifndef _IMAPOBJECT_H_
#define _IMAPOBJECT_H_

#include "../Game/IObject.h"
#include "../Core/Handles.h"

class IMapObject : public Game::IObject
{
	public:

		IMapObject() : IObject(OBJ_MAP) {}
		virtual ~IMapObject() {}

		virtual void Clear()=0;
		virtual bool Generate(const char *szName, const char *szBurnedMesh)=0;
		virtual bool Visibility(const IViewFrustrum *pFrustrum)=0;

		virtual bool DrawAmbient()=0;
		virtual bool DrawSunLight()=0;
		virtual bool DrawLight()=0;
		virtual bool DrawSpecial()=0;
		virtual bool Update(const float fTime)=0;

		virtual bool CastSunShadows(const Core::Vector3& vSunDir)=0;

		virtual bool Parse(FILE *pFile)=0;
		virtual bool Save(FILE *pFile)=0;

		virtual int GetRadius() const=0;
		virtual float GetRealRadius() const=0;

		virtual void SetMapPos(const Core::Vector2i& vMPos)=0;
		virtual const Core::Vector2i& GetMapPos() const=0;

		virtual Core::HMesh GetMesh()=0;

		virtual float GetHeight() const=0;
};

#endif