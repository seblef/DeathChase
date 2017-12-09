
#ifndef _GAMEEVENT_H_
#define _GAMEEVENT_H_

#include "../Core/Vector.h"

namespace Game
{

enum GameEventType
{
	GE_USER=0,
	GE_COLLIDE,

	GE_COUNT

};

class GameEvent
{
	public:

		GameEvent() : m_Type(GE_USER),m_dwSenderID(INVALID_ID),m_pParam(0) {}
		GameEvent(GameEventType _Type, uint32 dwSenderID, void *pParam=0) : m_Type(_Type),m_dwSenderID(dwSenderID),m_pParam(0) {}
		GameEvent(const GameEvent& e) : m_Type(e.m_Type),m_dwSenderID(e.m_dwSenderID),
			m_fParam(e.m_fParam),m_iParam(e.m_iParam),m_pParam(e.m_pParam),m_vParam(e.m_vParam) {}
		~GameEvent() {};

		void Clear();
		void Reset();

		void SetType(const GameEventType _Type) { m_Type=_Type; }
		GameEventType GetType() const { return m_Type; }

		void SetFParam(const float fParam) { m_fParam=fParam; }
		float GetFParam() const { return m_fParam; }

		void SetIParam(const int iParam) { m_iParam=iParam; }
		int GetIParam() const { return m_iParam; }

		void SetVParam(const Core::Vector3& vParam) { m_vParam=vParam; }
		const Core::Vector3& GetVParam() const { return m_vParam; }

		void SetPParam(void *pParam) { m_pParam=pParam; }
		const void *GetPParam() const { return m_pParam; }

		void SetSender(const uint32 dwSenderID) { m_dwSenderID=dwSenderID; }
		const uint32 GetSender() const { return m_dwSenderID; }

	protected:

		GameEventType m_Type;
		uint32 m_dwSenderID;

		float m_fParam;
		int m_iParam;
		void *m_pParam;
		Core::Vector3 m_vParam;
};


}

#endif