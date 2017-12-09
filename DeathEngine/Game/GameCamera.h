
#ifndef _GAMECAMERA_H_
#define _GAMECAMERA_H_

#include "../Core/Camera.h"

#define MAX_TRACE_CAMERA		40

namespace Game
{

class GameCamera : public Core::Camera
{
	public:

		enum GameCamState
		{
			CAMSTATE_TURNAROUND,
			CAMSTATE_FOLLOWSHIP,
			CAMSTATE_FOLLOWTARGET,

			CAMSTATE_COUNT

		};

		GameCamera() : m_iState(CAMSTATE_FOLLOWTARGET), m_fTime(0.0f),
			m_iTraceCount(0) {}
		GameCamera(const Core::Camera& cam) : Camera(cam),m_fTime(0.0f),
			m_iTraceCount(0) {}
		~GameCamera() {}

		void ActiveTurnAround(const Core::Vector3& vTarget,
			const float fHeight,
			const float fDist,
			const float fSpeed);
		void ActiveFollowShip();
		void ActiveFollowTarget(const Core::Vector3& vPos);

		void UpdateTurnAround(const float fTime);
		void UpdateFollowShip(const float fTime, const Core::Vector3& vShipPos, const Core::Quaternion& qRot);
		void AddTrajectoryPoint(const Core::Vector3& vPos, const Core::Quaternion& qRot);

		void UpdateFollowTarget(const Core::Vector3& vTargetPos);

	protected:

		virtual void RebuildViewMatrix();

		GameCamState m_iState;

		Core::Quaternion m_qShipRotation[MAX_TRACE_CAMERA];
		Core::Vector3 m_vShipPosition[MAX_TRACE_CAMERA];
		Core::Vector3 m_vUp;

		float m_fSpeed;
		float m_fHeight;
		float m_fDistance;
		float m_fCurrentAngle;

		float m_fTime;

		int m_iCurrentTrace;
		int m_iTraceCount;
};

}

#endif