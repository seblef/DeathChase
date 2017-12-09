
#ifndef _TOOLMANAGER_H_
#define _TOOLMANAGER_H_

#include "ITool.h"
#include "../../DeathEngine/Core/TSingleton.h"

enum ToolType
{
	TOOL_SELECT=0,
	TOOL_CAMZOOM,
	TOOL_CAMMOVE,
	TOOL_CAMROTATE,
	TOOL_CAMZMOVE,
	TOOL_TRANSLATE,
	TOOL_ROTATE,
	TOOL_SCALE,
	TOOL_LINK,
	TOOL_ALIGN,

	TOOL_COUNT
};


class ToolMgr : public Core::TSingleton<ToolMgr>
{
	public:

		ToolMgr();
		~ToolMgr();

		bool SetTool(ToolType _Type);
		ToolType GetCurrentToolType() const { return m_CurrentTool; }
		ITool *GetCurrentTool() { return m_pTools[m_CurrentTool]; }

		bool Start(ICamera *pCam, int x, int y,
			bool bLockX=false, bool bLockY=false, bool bLockZ=false);
		int MouseMove(const int dx, const int dy);
		void End(int x, int y);
		void Update();
		void Draw();
		HCURSOR GetCursor();

	protected:

		ITool *m_pTools[TOOL_COUNT];
		ToolType m_CurrentTool;

};

#define g_ToolMgr				ToolMgr::GetSingletonRef()

#endif