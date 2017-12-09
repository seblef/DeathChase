
#ifndef _SHADERMAP_H_
#define _SHADERMAP_H_

#include "../../DeathEngine/Core/TSingleton.h"
#include "../../DeathEngine/Common.h"
#include <map>
#include <string>

#define SMAP_SPECULAR					(1)
#define SMAP_EMISSIVE					(1 << 1)
#define SMAP_LIGHTED					(1 << 2)

#define SMAP_TEXBASE					(1 << 3)
#define SMAP_TEXSECADD					(1 << 4)
#define SMAP_TEXSECFILTER				(1 << 5)
#define SMAP_TEXSECFILTER2X				(1 << 6)

#define SMAP_BUMP						(1 << 7)
#define SMAP_ENVIRONMENT				(1 << 8)

#define SMAP_COUNT						((1 << 8) + 1)


#define SMAP_SPECULAR_EXT				"Spec"
#define SMAP_EMISSIVE_EXT				"Emiss"
#define SMAP_LIGHTED_EXT				"Light"

#define SMAP_TEXBASE_EXT				"Base"
#define SMAP_TEXSECADD_EXT				"Add"
#define SMAP_TEXSECFILTER_EXT			"Filter"
#define SMAP_TEXSECFILTER2X_EXT			"Filter2X"

#define SMAP_BUMP_EXT					"Bump"
#define SMAP_ENVIRONMENT_EXT			"Env"

class ShaderMap : public Core::TSingleton<ShaderMap>
{
	public:

		ShaderMap() {}
		~ShaderMap() {}

		void Release();
		void Create();

		const char *GetShaderName(uint32 dwFlags);

	protected:

		std::map<uint32,std::string> m_Map;

};

#define g_ShaderMap					ShaderMap::GetSingletonRef()

#endif