
#ifndef _SHADER_H_
#define _SHADER_H_

#include "IResource.h"
#include "../Dev/MemDebugger.h"

#ifdef DEEXPORT
#include "../Export/IShader.h"
#else
#define IShader				Core::Shader
#endif

struct IDirect3DVertexShader9;
struct IDirect3DPixelShader9;
struct ID3DXConstantTable;
struct ID3DXConstantTable;
struct IDirect3DDevice9;
namespace Core
{

#ifdef DEEXPORT
class Shader : public IShader, public IResource
#else
class Shader : public IResource
#endif
{
	public:

		Shader(IDirect3DDevice9 *pDevice);
		~Shader();

		bool Load(const char *szName);
		bool Set();
		void Release();

		IDirect3DVertexShader9 *GetVertexShader() { return m_pVShader; }
		IDirect3DPixelShader9 *GetPixelShader() { return m_pPShader; }
		const char *GetName() const { return m_szName; }

		bool BeginRestart();
		bool EndRestart(IDirect3DDevice9 *pDevice);

		static void SetShadersVersions(int iVersion) { m_siShaderVersion=iVersion; }

		bool SetConstant(bool bVertexShader, const char *szName,const float *pConstant,int iCount);

		MEMDEBUG_IMPL
		MEMDEBUG_CLASSNAME("Shader")
		MEMDEBUG_CLASSSIZE(sizeof(Shader))

	protected:

		bool LoadVShader(const char *szName);
		bool LoadPShader(const char *szName);

		IDirect3DVertexShader9 *m_pVShader;
		IDirect3DPixelShader9 *m_pPShader;
		ID3DXConstantTable *m_pVSConstantTable;
		ID3DXConstantTable *m_pPSConstantTable;
		IDirect3DDevice9 *m_pDevice;

		char m_szName[64];

		static int m_siShaderVersion;
};

}

#endif