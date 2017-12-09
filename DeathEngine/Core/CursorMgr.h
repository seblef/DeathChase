
#ifndef _CURSORMGR_H_
#define _CURSORMGR_H_

#include "TSingleton.h"
#include "Handles.h"
#include "../Dev/MemDebugger.h"
#include <map>
#include <string>

struct IDirect3DDevice9;

namespace Core
{

class CursorMgr : public TSingleton<CursorMgr>
{
	public:

		CursorMgr(IDirect3DDevice9 *pDevice);
		~CursorMgr();

		bool SetCursor(const char *szCursorName);
		bool AddCursor(const char *szName, const char *szFile, int iPosX=0, int iPosY=0);
		void Enable(bool bEnable);
		bool IsEnable() const { return m_bEnable; }
		void Update();

		void Release();

	private:

		class Cursor
		{
			public:

				Cursor() : iPosX(0),iPosY(0) {}
				Cursor(const Cursor& cur) : hTexture(cur.hTexture), iPosX(cur.iPosX), iPosY(cur.iPosY) {}
				~Cursor();

				const Cursor& operator=(const Cursor& cur)
				{
					iPosX=cur.iPosX;
					iPosY=cur.iPosY;
					hTexture=cur.hTexture;
				}

				HTexture hTexture;
				int iPosX,iPosY;

				MEMDEBUG_IMPL
				MEMDEBUG_CLASSNAME("Cursor")
				MEMDEBUG_CLASSSIZE(sizeof(Cursor))
		};

		void Load();

		bool m_bEnable;
		int m_iSizeX,m_iSizeY;
		IDirect3DDevice9 *m_pDevice;
		std::map<std::string,Cursor> m_Cursors;
		Cursor *m_pCurrent;
};

}

#define g_CursorMgr				Core::CursorMgr::GetSingletonRef()

#define CURSOR_SELECT			"select"
#define CURSOR_ALIGN			"align"
#define CURSOR_TRANSLATE		"translate"
#define CURSOR_SCALE			"scale"
#define CURSOR_ROTATE			"rotate"
#define CURSOR_LINK				"link"

#endif