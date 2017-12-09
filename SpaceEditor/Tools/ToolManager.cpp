
#include "ToolManager.h"
#include "ViewTools.h"
#include "ObjectTools.h"

ToolMgr::ToolMgr() : m_CurrentTool(TOOL_SELECT)
{
	m_pTools[TOOL_SELECT]=new ToolSelect;
	m_pTools[TOOL_CAMZOOM]=new ToolCamZoom;
	m_pTools[TOOL_CAMMOVE]=new ToolCamMove;
	m_pTools[TOOL_CAMROTATE]=new ToolCamRotate;
	m_pTools[TOOL_CAMZMOVE]=new ToolCamZMove;
	m_pTools[TOOL_TRANSLATE]=new ToolTranslate;
	m_pTools[TOOL_ROTATE]=new ToolRotate;
	m_pTools[TOOL_SCALE]=new ToolScale;
	m_pTools[TOOL_LINK]=new ToolNull;
	m_pTools[TOOL_ALIGN]=new ToolNull;
}

ToolMgr::~ToolMgr()
{
	for(int i=0;i<TOOL_COUNT;i++)
		if(m_pTools[i]) delete m_pTools[i];
}

bool ToolMgr::SetTool(ToolType _Type)
{
	if(_Type < 0 || _Type >= TOOL_COUNT)
		return false;
	else
	{
		m_CurrentTool=_Type;
		return true;
	}
}

bool ToolMgr::Start(ICamera *pCam, int x, int y, bool bLockX, bool bLockY, bool bLockZ)
{
	if(m_pTools[m_CurrentTool])
		return m_pTools[m_CurrentTool]->Start(pCam,x,y,bLockX,bLockY,bLockZ);
	else
		return false;
}

int ToolMgr::MouseMove(const int dx, const int dy)
{
	if(m_pTools[m_CurrentTool])
		return m_pTools[m_CurrentTool]->MouseMove(dx,dy);
	else
		return 0;
}

void ToolMgr::End(int x, int y)
{
	if(m_pTools[m_CurrentTool])
		m_pTools[m_CurrentTool]->End(x,y);
}

void ToolMgr::Update()
{
	if(m_pTools[m_CurrentTool])
		m_pTools[m_CurrentTool]->Update();
}

void ToolMgr::Draw()
{
	if(m_pTools[m_CurrentTool])
		m_pTools[m_CurrentTool]->Draw();
}

HCURSOR ToolMgr::GetCursor()
{
	if(m_pTools[m_CurrentTool])
		return m_pTools[m_CurrentTool]->GetCursor();
	else
		return AfxGetApp()->LoadStandardCursor(IDC_ARROW);
}

