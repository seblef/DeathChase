
#ifdef DEEXPORT

#include "Factory.h"
#include "../Core/BMPImage.h"
#include "../Core/Camera.h"
#include "../Game/Heightmap.h"
#include "../Game/PatchGrid.h"
#include "../Core/ScriptFile.h"
#include "../Core/ViewFrustrum.h"
#include "../Game/SpaceShip.h"

Factory::Factory()
{
	for(int i=0;i<EOBJ_COUNT;i++)
		m_Allocators[i]=new Core::TMemoryAllocator<IExportObject>(ExportObjectClassNames[i]);
}

Factory::~Factory()
{
	for(int i=0;i<EOBJ_COUNT;i++)
		delete m_Allocators[i];
}

IExportObject *Factory::CreateObject(ExportObjectType _Type)
{
	MYASSERT(_Type >= 0 && _Type < EOBJ_COUNT,"Mauvais type d'objet à exporter",true);

	IExportObject *pRet=m_Allocators[_Type]->GetFree();
	if(pRet) return pRet;

	switch(_Type)
	{
		case EOBJ_BMPIMAGE: pRet=new Core::BMPImage; break;
		case EOBJ_CAMERA: pRet=new Core::Camera; break;
		case EOBJ_SCRIPTFILE: pRet=new Core::ScriptFile; break;
		case EOBJ_VIEWFRUSTRUM: pRet=new Core::ViewFrustrum; break;
		case EOBJ_SPACESHIP: pRet=new Game::SpaceShip; break;
	}

	if(pRet) m_Allocators[_Type]->RegisterNew(pRet);
	return pRet;
}

void Factory::ReleaseObject(IExportObject *pObj)
{
	if(pObj)
		m_Allocators[pObj->GetType()]->Delete(pObj);
}

#endif