
#ifndef _WINDOWMGR_H_
#define _WINDOWMGR_H_

#include "stdafx.h"
#include "../DeathEngine/Core/TSingleton.h"

enum StdWindowType
{
	WT_NEWMAP=0,
	WT_HEIGHTMAP,
	WT_TEXTURE,
	WT_ENVIRONMENT,
	WT_WATER,
	WT_TREES,

	WT_COUNT

};

class WindowMgr : public Core::TSingleton<WindowMgr>
{
	public:

		WindowMgr();
		~WindowMgr() { DeleteAllDialogs(); }

		void ShowDialog(StdWindowType _Type, bool bReinit=true);
		void HideDialog(StdWindowType _Type);
		
		void CloseAllDialogs();
		void DeleteAllDialogs();

		void SetDoc(CDocument *pDoc) { m_pDocument=pDoc; }
		CDocument *GetDoc() { return m_pDocument; }

		CDialog *GetDialog(StdWindowType _Type);
		void ReinitAllDialogs();

	protected:

		CDialog *m_pDialogs[WT_COUNT];
		CDocument *m_pDocument;
};

#define g_WindowMgr				WindowMgr::GetSingletonRef()

#endif