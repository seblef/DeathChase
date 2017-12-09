
#ifndef _SUN_H_
#define _SUN_H_

#ifdef DEEXPORT
#include "../Export/ISun.h"
#else
#include "../Core/Vector.h"
#include "../Core/Handles.h"
#include "../Core/Color.h"

#define ISun			Game::Sun

#endif

namespace Game
{

#ifdef DEEXPORT
class Sun : public ISun
#else
class Sun
#endif
{
	public:

		Sun();
		~Sun();

		void Clear();

		bool Draw();
		bool Generate(Core::HTexture hTex, Core::Color32f cColor, Core::Color32f cAmbientLight,
			Core::Vector3& vDir);

		void Set();

		void SetTexture(Core::HTexture hTex) { m_hTexture=hTex; }
		Core::HTexture GetTexture() const { return m_hTexture; }

		void SetDirection(const Core::Vector3& vDir) { m_vDirection=vDir; }
		const Core::Vector3& GetDirection() const { return m_vDirection; }

		void SetColor(const Core::Color32f& cColor) { m_cColor=cColor; }
		const Core::Color32f& GetColor() const;// { return m_cColor; }

		void SetAmbientLight(const Core::Color32f& cAmbient) { m_cAmbientLight=cAmbient; }
		const Core::Color32f& GetAmbientLight() const { return m_cAmbientLight; }

	protected:

		Core::Color32f m_cColor;
		Core::Color32f m_cAmbientLight;
		Core::Vector3 m_vDirection;

		Core::HTexture m_hTexture;
};

}

#endif