
#ifndef _KEYBUTTON_H_
#define _KEYBUTTON_H_

#include "IKeyButton.h"
#include "../Dev/MemDebugger.h"

namespace GUI
{

class KeyButton : public IKeyButton
{
	public:

	  KeyButton(IEnvironment *pEnvironment, IElement *pParent, IEventReceiver *pUser,
			uint32 dwID, Core::Rect rect, bool bNoClip=false);
	  virtual ~KeyButton();

	  virtual void Reset(IEnvironment *pEnvironment, IElement *pParent,
		  IEventReceiver *pUser, uint32 dwID, Core::Rect rect);
	  virtual void Clear();

	  virtual bool OnEvent(const Event& _Event);
	  virtual void Draw();

	  virtual void SetKey(const byte byKey);
	  virtual byte GetKey() const { return m_byKey; }

	  MEMDEBUG_IMPL
	  MEMDEBUG_CLASSNAME("KeyButton")
	  MEMDEBUG_CLASSSIZE(sizeof(KeyButton))

	private:

		bool m_bPressed;
		byte m_byKey;

};

}

#endif