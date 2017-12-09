
#ifndef _SKYBOX_H_
#define _SKYBOX_H_

#ifdef DEEXPORT
#include "../Export/ISkyBox.h"
#else
#include "../Core/Handles.h"
#endif

#include "../Core/Vertex.h"

struct IDirect3DDevice9;

#ifndef DEEXPORT
enum SkyFace
{
	SKY_UP,
	SKY_FRONT,
	SKY_RIGHT,
	SKY_BACK,
	SKY_LEFT,

	SKY_COUNT
};

#define ISkyBox				Game::SkyBox

#endif

namespace Game
{

#ifdef DEEXPORT
class SkyBox : public ISkyBox
#else
class SkyBox
#endif
{
	public:


		SkyBox();
		~SkyBox();

		bool Create(IDirect3DDevice9 *pDevice, const float fSize);
		void Clear();
		bool SetTexture(SkyFace iFace, Core::HTexture hTex);
		Core::HTexture GetTexture(SkyFace iFace);
		bool Draw(const Core::Vector3& vCamPos);

	private:

		IDirect3DDevice9 *m_pDevice;
		Core::VertexPT m_Vertices[SKY_COUNT*4];
		uint16 m_wIndices[6];
		Core::HTexture m_hTextures[SKY_COUNT];
		Core::HShader m_hShader;
};

}

#endif