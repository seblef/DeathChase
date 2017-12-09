
#ifndef _EDITORSUN_H_
#define _EDITORSUN_H_

#include "IEditorObject.h"
#include "../../DeathEngine/Core/Handles.h"

struct IDirect3DIndexBuffer9;
struct IDirect3DVertexBuffer9;

class EditorSun : public IEditorObject
{
	public:

		EditorSun();
		~EditorSun();

		bool Draw();
		bool Load(const char *szName, const char *szPath);

		void Translate(const Core::Vector3& v);

	protected:

		int m_iTriCount;
		int m_iVertCount;

		IDirect3DIndexBuffer9 *m_pIBuf;
		IDirect3DVertexBuffer9 *m_pVBuf;

		Core::HShader m_hShader;
		Core::HMesh m_hMesh;

};

#endif