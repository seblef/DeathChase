
#ifndef _VIEWTOOLS_H_
#define _VIEWTOOLS_H_

#include "ITool.h"
#include "../DeathEditor.h"

class ToolCamZoom : public ITool
{
	public:

		ToolCamZoom() : m_pCamera(0) {}
		~ToolCamZoom() {}

		bool Start(ICamera *pCam,
			int x, int y,
			bool bLockX=false, bool bLockY=false, bool bLockZ=false);
		int MouseMove(const int dx, const int dy);
		void End(int x, int y) {}
		void Update() {}
		void Draw() {}
		HCURSOR GetCursor() { return AfxGetApp()->LoadCursor(IDC_ZOOMCUR); }

	protected:

		ICamera *m_pCamera;
};

class ToolCamMove : public ITool
{
	public:

		ToolCamMove() : m_pCamera(0) {}
		~ToolCamMove() {}

		bool Start(ICamera *pCam,
			int x, int y,
			bool bLockX=false, bool bLockY=false, bool bLockZ=false);
		int MouseMove(const int dx, const int dy);
		void End(int x, int y) {}
		void Update() {}
		void Draw() {}
		HCURSOR GetCursor() { return AfxGetApp()->LoadCursor(IDC_CAMMOVECUR); }

	protected:

		ICamera *m_pCamera;
};

class ToolCamRotate : public ITool
{
	public:

		ToolCamRotate() : m_pCamera(0) {}
		~ToolCamRotate() {}

		bool Start(ICamera *pCam,
			int x, int y,
			bool bLockX=false, bool bLockY=false, bool bLockZ=false);
		int MouseMove(const int dx, const int dy);
		void End(int x, int y) {}
		void Update() {}
		void Draw() {}
		HCURSOR GetCursor() { return AfxGetApp()->LoadCursor(IDC_CAMROTATECUR); }

	protected:

		ICamera *m_pCamera;
};

class ToolCamZMove : public ITool
{
	public:

		ToolCamZMove() : m_pCamera(0) {}
		~ToolCamZMove() {}

		bool Start(ICamera *pCam,
			int x, int y,
			bool bLockX=false, bool bLockY=false, bool bLockZ=false);
		int MouseMove(const int dx, const int dy);
		void End(int x, int y) {}
		void Update() {}
		void Draw() {}
		HCURSOR GetCursor() { return AfxGetApp()->LoadCursor(IDC_ZOOMCUR); }

	protected:

		ICamera *m_pCamera;
};

#endif