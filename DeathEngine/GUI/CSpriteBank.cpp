
#include "CSpriteBank.h"
#include "../Core/TextureMgr.h"
#include "../Core/Renderer2D.h"

namespace GUI
{

SpriteBank::SpriteBank(IEnvironment *pEnvironment) : m_pEnvironment(pEnvironment)
{
}

SpriteBank::~SpriteBank()
{
	int i;
	for(i=0;i<m_Textures.size();i++)
		g_TextureMgr.ReleaseTexture(m_Textures[i]);
}

std::vector<Core::Rect>& SpriteBank::GetPositions()
{
	return m_Rects;
}

std::vector<Sprite>& SpriteBank::GetSprites()
{
	return m_Sprites;
}

uint32 SpriteBank::GetTextureCount() const
{
	return m_Textures.size();
}

Core::HTexture SpriteBank::GetTexture(const uint32 dwIndex) const
{
	if(dwIndex < m_Textures.size())
		return m_Textures[dwIndex];
	else
		return Core::HTexture();
}

void SpriteBank::AddTexture(Core::HTexture hTex)
{
	if(hTex.IsNull())
		m_Textures.push_back(Core::HTexture());
	else
		m_Textures.push_back(hTex);
}

void SpriteBank::SetTexture(uint32 dwIndex, Core::HTexture hTex)
{
	while(dwIndex > m_Textures.size())
		m_Textures.push_back(Core::HTexture());

	if(!m_Textures[dwIndex].IsNull())
		g_TextureMgr.ReleaseTexture(m_Textures[dwIndex]);

	m_Textures[dwIndex]=hTex;
}

void SpriteBank::DrawSprite(uint32 dwIndex, const Core::Vector2i& vPos,
							const Core::Rect *pClip, const Core::Color32& cColor,
							uint32 dwStartTime, uint32 dwCurrentTime,
							bool bLoop, bool bCenter)
{
	if(dwIndex >= m_Sprites.size() || m_Sprites[dwIndex].Frames.empty())
		return;

	uint32 dwFrame=0;
	if(m_Sprites[dwIndex].dwFrameTime)
	{
		uint32 f=((dwCurrentTime - dwStartTime) / m_Sprites[dwIndex].dwFrameTime);
		if(bLoop)
			dwFrame=f % m_Sprites[dwIndex].Frames.size();
		else
			dwFrame=(f >= m_Sprites[dwIndex].Frames.size()) ? m_Sprites[dwIndex].Frames.size() : f;
	}

	const Core::HTexture hTex(m_Textures[m_Sprites[dwIndex].Frames[dwFrame].wTextureNum]);
	if(hTex.IsNull())
		return;

	const uint32 rn=m_Sprites[dwIndex].Frames[dwFrame].wRectNum;
	if(rn >= m_Rects.size())
		return;

	const Core::Rect& r=m_Rects[rn];

	if(bCenter)
	{
		Core::Vector2i p(vPos);
		p-=r.GetSize() / 2;
		g_Renderer2D.Draw2DImage(hTex,p,r,pClip,cColor,true);
	}
	else
		g_Renderer2D.Draw2DImage(hTex,vPos,r,pClip,cColor,true);
}

}