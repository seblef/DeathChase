
#ifndef _MESHLOADER_H_
#define _MESHLOADER_H_

#include "../../DeathEngine/Core/TSingleton.h"
#include "../../DeathEngine/Export/IMeshMgr.h"
#include "../stdafx.h"
#include <vector>
#include <string>

class ILoader;
class IMaterial;
class LMaterial;

class MeshLoader : public Core::TSingleton<MeshLoader>
{
	public:

		MeshLoader() {}
		~MeshLoader() { Release(); }

		bool Init();
		bool Release();

		ILoader *LoadMesh(const char *szFileName, const char *szPath, const char *szExtension);
		void ConvertMaterial(IMaterial *pDestMat, LMaterial *pSrcMat, const char *szTexPath=0);

		const std::string& GetExtensions() const { return m_strExtensions; }
		const std::string& GetExtPath() const { return m_strExtPath; }
		std::vector<std::string> *GetFilters() { return &m_Filters; }

	protected:

		class Loader
		{
			public:

				HINSTANCE hDLL;
				ILoader *pLoader;
		};

		std::vector<Loader> m_Loaders;
		std::vector<std::string> m_Filters;
		std::string m_strExtensions;
		std::string m_strExtPath;

};

#define g_MeshLoader				MeshLoader::GetSingletonRef()

#endif