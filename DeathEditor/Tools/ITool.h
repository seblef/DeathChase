
#ifndef _ITOOL_H_
#define _ITOOL_H_

#include "../stdafx.h"
#include "../../DeathEngine/Export/ICamera.h"

#define TR_REDRAWMAINVIEW				1
#define TR_CREATEOBJECT					2

class ITool
{
	public:

		ITool() {}
		virtual ~ITool() {}

		virtual bool Start(ICamera *pCam,
			int x, int y,
			bool bLockX=false, bool bLockY=false, bool bLockZ=false)=0;
		virtual int MouseMove(const int dx, const int dy)=0;
		virtual void End(int x, int y)=0;
		virtual void Update()=0;
		virtual void Draw()=0;
		virtual HCURSOR GetCursor()=0;

};

class ToolNull : public ITool
{
	public:

		ToolNull() {}
		~ToolNull() {}

		bool Start(ICamera *pCam,
			int x, int y,
			bool bLockX=false, bool bLockY=false, bool bLockZ=false) { return true; }
		int MouseMove(const int dx, const int dy) { return 0; }
		void End(int x, int y) {}
		void Update() {}
		void Draw() {}
		HCURSOR GetCursor() { return AfxGetApp()->LoadStandardCursor(IDC_ARROW); }

};

#endif