
#include "CSplitterWndEx.h"

BEGIN_MESSAGE_MAP(CSplitterWndEx, CSplitterWnd)
//{{AFX_MSG_MAP(CSplitterWndEx)
ON_WM_SETCURSOR()
ON_WM_MOUSEMOVE()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CSplitterWndEx::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
    return FALSE;
}

void CSplitterWndEx::OnMouseMove(UINT nFlags, CPoint point) 
{
}