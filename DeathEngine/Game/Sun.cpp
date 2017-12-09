
#include "Sun.h"
#include "../Core/TextureMgr.h"
#include "../Core/Renderer.h"

namespace Game
{

Sun::Sun() : m_vDirection(0.0f,-1.0f,0.0f),
	m_cAmbientLight(1.0f,0.5f,0.5f,0.5f),
	m_cColor(1.0f,0.8f,0.8f,0.8f)
{
}

Sun::~Sun()
{
	g_TextureMgr.ReleaseTexture(m_hTexture);
}

void Sun::Clear()
{
	g_TextureMgr.ReleaseTexture(m_hTexture);
	m_vDirection=Core::Vector3(0.0f,-1.0f,0.0f);
	m_cAmbientLight=Core::Color32f(1.0f,0.5f,0.5f,0.5f);
	m_cColor=Core::Color32f(1.0f,0.8f,0.8f,0.8f);
}

bool Sun::Draw()
{
	if(!m_hTexture.IsNull())
		return false;

	return true;
}

bool Sun::Generate(Core::HTexture hTex, Core::Color32f cColor, Core::Color32f cAmbientLight,
				   Core::Vector3 &vDir)
{
	m_vDirection=vDir;
	m_cColor=cColor;
	m_cAmbientLight=cAmbientLight;
	m_hTexture=hTex;

	return true;
}

void Sun::Set()
{
	const float fAmbient[4]={ m_cAmbientLight.fr, m_cAmbientLight.fg, m_cAmbientLight.fb, 1.0f};
	const float fColor[4]={ m_cColor.fr, m_cColor.fg, m_cColor.fb, 1.0f};
	const float fDir[4]={ m_vDirection.x, m_vDirection.y, m_vDirection.z, 0.0f };

	g_Renderer.SetVertexShaderConstant(COLOR_AMBIENT_REG, fAmbient);
	g_Renderer.SetVertexShaderConstant(LIGHT_SUN_COLOR_REG, fColor);
	g_Renderer.SetVertexShaderConstant(LIGHT_SUN_DIRECTION_REG, fDir);
}

const Core::Color32f& Sun::GetColor() const
{
	return m_cColor;
}

}