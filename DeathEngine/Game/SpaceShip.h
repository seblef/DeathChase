
#ifndef _SPACESHIP_H_
#define _SPACESHIP_H_

#include "../Core/Handles.h"
#include "../Core/Quaternion.h"
#include "../Core/ShadowVolume.h"
#include "ISpaceShipController.h"
#include "GameCamera.h"
#include "IObject.h"
#include <vector>

#ifdef DEEXPORT
#include "../Export/ISpaceShip.h"
#endif

namespace Game
{

class Explosion;

#ifdef DEEXPORT
class SpaceShip : public ISpaceShip
#else
class SpaceShip : public IObject
#endif
{
	public:

		SpaceShip();
		~SpaceShip();

		void Clear();

		bool Load(const char *szFileName, const char *szPath);
		bool Save(const char *szFileName, const char *szPath);
		bool Update(const float fTime);
		
		bool DrawAmbient();
		bool DrawSunLight();
		bool DrawSpecial();

		void SetController(ISpaceShipController *pController) { m_pController=pController; };
		void SetCamera(GameCamera *pCamera) { m_pCamera=pCamera; };

		const Core::Vector3& GetDirection() const { return m_vDirection; };
		const Core::Vector3& GetLocalXAxis() const { return m_vLocalXAxis; };
		const Core::Matrix4& GetRotationMatrix() const { return m_mRotation; };
		bool IsFlying() const { return m_iState==0; };
		float GetSpeed() const { return m_fSpeed; };

		void OnEvent(const GameEvent& Event);

		bool CastSunShadows(const Core::Vector3& vSunDir);

		void AddMainFirePoint(const Core::Vector3& vFire);
		void AddSecFirePoint(const Core::Vector3& vFire);
		void AddBackFirePoint(const Core::Vector3& vFire);
		void AddDefensePoint(const Core::Vector3& vDefense);
		void AddBombDropPoint(const Core::Vector3& vDrop);

		int GetMainFirePointCount() const { return m_MainFirePoints.size(); }
		int GetSecFirePointCount() const { return m_SecFirePoints.size(); }
		int GetBackFirePointCount() const { return m_BackFirePoints.size(); }
		int GetDefensePointCount() const { return m_DefensePoints.size(); }
		int GetBombDropPointCount() const { return m_BombDropPoints.size(); }

		Core::Vector3 GetMainFirePoint(const int iIndex);
		Core::Vector3 GetSecFirePoint(const int iIndex);
		Core::Vector3 GetBackFirePoint(const int iIndex);
		Core::Vector3 GetDefensePoint(const int iIndex);
		Core::Vector3 GetBombDropPoint(const int iIndex);

		void SetMesh(Core::HMesh hMesh);
		Core::HMesh GetMesh() const { return m_hMesh; }

		void SetLinearSpeed(const float fSpeed) { m_fSpeed=fSpeed; }
		float GetLinearSpeed() const { return m_fSpeed; }

		void SetRotationSpeed(const float fSpeed) { m_fRotSpeed=fSpeed; }
		float GetRotationSpeed() const { return m_fRotSpeed; }

	protected:

		void InitializeShipInfo(void *pInfo);
		void Collision(const bool bOnWater=false);

		ISpaceShipController *m_pController;

		float m_fRotSpeed;
		float m_fSpeed;

		Core::Vector3 m_vDirection;
		Core::Vector3 m_vLocalXAxis;
		Core::Matrix4 m_mRotation;

		GameCamera *m_pCamera;
		float m_fTime;

		int m_iState;

		Core::HMesh m_hMesh;
		Core::ShadowVolume m_SunShadows;

		std::vector<Core::Matrix4> m_MainFirePoints;
		std::vector<Core::Matrix4> m_SecFirePoints;
		std::vector<Core::Matrix4> m_DefensePoints;
		std::vector<Core::Matrix4> m_BombDropPoints;
		std::vector<Core::Matrix4> m_BackFirePoints;

		int m_iLastMainFirePoint;
		int m_iLastSecFirePoint;
		int m_iLastDefensePoint;
		int m_iLastBombDropPoint;
		int m_iLastBackFirePoint;

		Explosion *m_pExplosion;
};

}

#endif