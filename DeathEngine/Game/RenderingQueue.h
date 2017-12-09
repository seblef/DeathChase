
#ifndef _RENDERINGQUEUE_H_
#define _RENDERINGQUEUE_H_

#include "IObject.h"
#include "../Core/TSingleton.h"
#include "../Core/Handles.h"
#include <vector>

namespace Core
{

class Camera;

}

namespace Game
{

class RenderingQueue : public Core::TSingleton<RenderingQueue>
{
	public:

		RenderingQueue();
		~RenderingQueue();

		void Register(IObject *pObject);
		void Execute(const Core::Vector3& vSunDir);

		void SetLightEnable(bool bLightEnable) { m_bLightEnable=bLightEnable; }
		void SetShadowsEnable(bool bShadowsEnable) { m_bShadowsEnable=bShadowsEnable; }

		bool IsLightEnable() const { return m_bLightEnable; }
		bool IsShadowsEnable() const { return m_bShadowsEnable; }

		void SetCamera(Core::Camera *pCamera) { m_pCamera=pCamera; }

	protected:

		typedef std::vector<IObject *> ObjectVector;

		ObjectVector m_AmbientSunPass;
		ObjectVector m_CastShadows;
		ObjectVector m_NoReceiveShadows;
		ObjectVector m_NoLightPass;
		ObjectVector m_SpecialPass;

		Core::HShader m_hShadowsShader;

		bool m_bShadowsEnable;
		bool m_bLightEnable;
		
		Core::Camera *m_pCamera;
};

}

#define g_RenderingQueue			Game::RenderingQueue::GetSingletonRef()

#endif