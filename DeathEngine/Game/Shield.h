
#ifndef _SHIELD_H_
#define _SHIELD_H_

#include "IObject.h"

#ifdef DEEXPORT
#include "../Export/IShield.h"
#else
#include "../Core/Handles.h"

#define IShield				Game::Shield

#endif

#include "IGameEventReceiver.h"

struct IDirect3DVertexBuffer9;
struct IDirect3DIndexBuffer9;

namespace Game
{

#ifdef DEEXPORT
class Shield : public IObject, public IShield
#else
class Shield : public IObject
#endif
{
	public:

		Shield();
		~Shield();

		bool Create(const Core::Vector3& vSize, int iMapSize, uint32 dwVisDist, Core::HTexture hTex);
		void Clear();

		Core::HTexture GetTexture() const { return m_hTexture; }
		void SetTexture(Core::HTexture hTex);

		Core::HShader GetShader() const { return m_hShader; }
		void SetShader(Core::HShader hShader);

		uint32 GetVisibilityDistance() const { return m_dwVisDist; }
		void SetVisibilityDistance(uint32 dwDist) { m_dwVisDist=dwDist; }

		const Core::Vector3& GetPosition() const { return m_vPosition; }
		const Core::Vector3& GetSize() const { return m_vSize; }
		void SetSize(const Core::Vector3& vSize);

		bool Draw();
		void OnEvent(const GameEvent& Event);

		void EnableShield(const bool bEnable) { m_bEnable=bEnable; }

		void SetCameraPosition(const Core::Vector3& vCamPos) { m_vCamPos=vCamPos; }

	protected:

		Core::HTexture m_hTexture;
		Core::HShader m_hShader;
		Core::HShader m_hNoTexShader;

		IDirect3DVertexBuffer9 *m_pVBuf;
		IDirect3DIndexBuffer9 *m_pIBuf;

		uint32 m_dwVertCount;
		uint32 m_dwTriCount;

		Core::Matrix4 m_mWorld;
		Core::Vector3 m_vPosition;
		Core::Vector3 m_vSize;

		Core::Vector3 m_vCamPos;

		uint32 m_dwVisDist;
		bool m_bEnable;

};

}

#endif