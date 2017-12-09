
#ifndef _OBJECTTOOLS_H_
#define _OBJECTTOOLS_H_

#include "ITool.h"
#include "../DeathEditor.h"
#include "../Data/Selector.h"

class IToolObject : public ITool
{
	public:

		IToolObject();
		~IToolObject() {};

		virtual void End(int x, int y);
		virtual int MouseMove(const int dx, const int dy);

	protected:

		void DoSelection(int x, int y, ICamera *pCam);

		ICamera *m_pCam;
		Game::IObject *m_pObj;

		int m_iLastStartX,m_iLastStartY;
		int m_iTotalMoveX, m_iTotalMoveY;

};


class ToolTranslate : public IToolObject
{
	public:

		ToolTranslate() : m_bValidPlace(true), m_bForced(false) {}
		~ToolTranslate() {}

		bool Start(ICamera *pCam,
			int x, int y,
			bool bLockX=false, bool bLockY=false, bool bLockZ=false);
		int MouseMove(const int dx, const int dy);
		void Update() {}
		void End(int x, int y);
		void Draw();

		void ForceObject(Game::IObject *pObj);

		HCURSOR GetCursor() { return AfxGetApp()->LoadCursor(IDC_TRANSLATECUR); }

	protected:

		int TranslateMapObject();

		bool m_bValidPlace;
		Core::Vector3 m_vSavedPos;
		Core::Vector2i m_vSavedMapPos;

		bool m_bLocked[3];

		Core::HShader m_hInvalidShader;

		bool m_bForced;
		bool m_bCreateObject;
};

class ToolRotate : public IToolObject
{
	public:

		ToolRotate() {}
		~ToolRotate() {}

		bool Start(ICamera *pCam, int x, int y,
			bool bLockX=false, bool bLockY=false, bool bLockZ=false);
		int MouseMove(const int dx, const int dy);
		void Update() {}
		void End(int x, int y) {}
		void Draw() {}

		HCURSOR GetCursor() { return AfxGetApp()->LoadCursor(IDC_ROTATEECUR); }

	protected:

		bool m_bLocked[3];

};

class ToolScale : public IToolObject
{
	public:

		ToolScale() {}
		~ToolScale() {}

		bool Start(ICamera *pCam, int x, int y,
			bool bLockX=false, bool bLockY=false, bool bLockZ=false);
		int MouseMove(const int dx, const int dy);
		void Update() {}
		void End(int x, int y) {}
		void Draw() {}

		HCURSOR GetCursor() { return AfxGetApp()->LoadCursor(IDC_SCALECUR); }
};

class ToolSelect : public IToolObject
{
	public:

		ToolSelect() {}
		~ToolSelect() {}

		bool Start(ICamera *pCam, int x, int y,
			bool bLockX=false, bool bLockY=false, bool bLockZ=false) { DoSelection(x,y,pCam); return true; }
		int MouseMove(const int dx, const int dy) { return 0; }
		void Update() {}
		void End(int x, int y) {}
		void Draw() {}

		HCURSOR GetCursor() { return AfxGetApp()->LoadStandardCursor(IDC_ARROW); }

};

#endif
