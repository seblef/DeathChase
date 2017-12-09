
#ifndef _IMESHOBJECT_H_
#define _IMESHOBJECT_H_

#include "../Game/IObject.h"
#include "../Core/Handles.h"

class IMeshObject : public Game::IObject
{
	public:

		IMeshObject() : IObject(OBJ_MESH) {}
		virtual ~IMeshObject() {}

		virtual void SetMesh(Core::HMesh hMesg)=0;
		virtual Core::HMesh GetMesh() const=0;

		virtual void SetAnimation(const int iAxe, const float fSpeed)=0;
		virtual void GetAnimation(int& iAxe, float& fSpeed) const=0;
		virtual bool IsAnimated() const=0;
		virtual void DisableAnimation()=0;
};

#endif