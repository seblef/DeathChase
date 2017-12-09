
#ifndef _SHADERMGR_H_
#define _SHADERMGR_H_

#include "Shader.h"
#include "THandleMgr.h"
#include "TSingleton.h"
#include <map>
#include <string>

#ifdef DEEXPORT
#include "../Export/IShaderMgr.h"
#endif

namespace Core
{

#ifdef DEEXPORT
class ShaderMgr : public IShaderMgr, public TSingleton<ShaderMgr>
#else
class ShaderMgr : public TSingleton<ShaderMgr>
#endif
{
	public:

		typedef THandleMgr<Shader *,HShader> HShaderMgr;

		ShaderMgr(IDirect3DDevice9 *pDevice);
		~ShaderMgr();

		HShader LoadShader(const char *szName);
		void ReleaseShader(HShader &hShader);

		bool SetShader(HShader hShader);

		bool BeginRestart();
		bool EndRestart(IDirect3DDevice9 *pDevice);

		IShader *GetShader(HShader hShader);

	protected:

		IDirect3DDevice9 *m_pDevice;
		HShader m_hLastShader;

		struct istring_less
		{
			bool operator() (const std::string& l, const std::string& r) const
			{
				return _stricmp(l.c_str(),r.c_str()) < 0;
			}
		};
	
		typedef std::map<std::string,HShader,istring_less> NameIndex;
		
		HShaderMgr m_Shaders;
		NameIndex m_NameIndex;

};

}

#define g_ShaderMgr				Core::ShaderMgr::GetSingletonRef()

#endif