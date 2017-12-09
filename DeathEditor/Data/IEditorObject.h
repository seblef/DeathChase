
#ifndef _IEDITOROBJECT_H_
#define _IEDITOROBJECT_H_

#include "../../DeathEngine/Game/IObject.h"

class IEditorObject : public Game::IObject
{
	public:

		IEditorObject() : IObject(OBJ_EDITOR) {}
		virtual ~IEditorObject() {}

		virtual void Select() {};
		virtual void Translate(const Core::Vector3& v) {}
		virtual void Rotate(const Core::Vector3& v) {}
		virtual void Scale(const Core::Vector3& v) {}

		virtual void OnEvent(const Game::GameEvent& Event) {}

};

#endif