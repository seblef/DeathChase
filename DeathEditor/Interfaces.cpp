
#include "Interfaces.h"
#include "../DeathEngine/Core/Matrix.h"

typedef bool (*GETINTERFACE)(IEngine **);

bool Interfaces::Load(HINSTANCE hAppInst)
{
#ifdef DEDEV
	m_hEngine=(HINSTANCE)::LoadLibrary("DEngineDev.dll");
#else
	m_hEngine=(HINSTANCE)::LoadLibrary("DEngine.dll");
#endif

	if(!m_hEngine)
	{
		MessageBox(0,"Impossible de trouver le fichier DEngine.dll","Erreur",MB_OK | MB_ICONERROR);
		return false;
	}

	GETINTERFACE pfLoader=(GETINTERFACE)::GetProcAddress(m_hEngine,"GetEngine");

	if(!pfLoader)
	{
		MessageBox(0,"L'extension DEngine.dll doit être corrompue, impossible de trouver la fonction GetEngine","Erreur",MB_OK | MB_ICONERROR);
		::FreeLibrary(m_hEngine);
		return false;
	}

	if(!pfLoader(&m_pEngine))
	{
		MessageBox(0,"Erreur lors de la création du moteur","Erreur",MB_OK | MB_ICONERROR);
		::FreeLibrary(m_hEngine);
		return false;
	}

	if(!m_pEngine->Start(hAppInst))
	{
		MessageBox(0,"Erreur lors de l'initialisation du moteur","Erreur",MB_OK | MB_ICONERROR);
		pfLoader=(GETINTERFACE)::GetProcAddress(m_hEngine,"ReleaseEngine");
		if(pfLoader) pfLoader(&m_pEngine);
		::FreeLibrary(m_hEngine);
		return false;
	}

	Core::Matrix4::InitTrigo();

	return true;
}

void Interfaces::Release()
{
	if(m_hEngine)
	{
		if(m_pEngine)
		{
			m_pEngine->ShutDown();

			GETINTERFACE pfLoader=(GETINTERFACE)::GetProcAddress(m_hEngine,"ReleaseEngine");
			if(pfLoader) pfLoader(&m_pEngine);
		}

		::FreeLibrary(m_hEngine);
		m_hEngine=0;
	}
}
