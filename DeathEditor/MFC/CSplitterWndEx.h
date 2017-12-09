
#ifndef _CSPLITTERWNDEX_H_
#define _CSPLITTERWNDEX_H_

#include "../stdafx.h"

#define PANE_BORDER 1
#define PANE_SHARE  0
#define PANE_GAP 1
        
class CSplitterWndEx : public CSplitterWnd
{
// Construction
public:
   CSplitterWndEx()
   {
   }
   void SetThinFrame()
   {
        m_cxSplitter=PANE_BORDER;     
        m_cySplitter=PANE_BORDER;     
        m_cxBorderShare=PANE_SHARE;   
        m_cyBorderShare=PANE_SHARE;   
        m_cxSplitterGap=PANE_GAP;     
        m_cySplitterGap=PANE_GAP;   
   }
// Operations
public:

    virtual void StartTracking(int ht)
    {
    } 

   // Overrides
// ClassWizard generated virtual function overrides
//{{AFX_VIRTUAL(CSplitterWndEx)
public:
//}}AFX_VIRTUAL

// Attributes
public:
   
// Generated message map functions
protected:
//{{AFX_MSG(CSplitterWndEx)
afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
afx_msg void OnMouseMove(UINT nFlags, CPoint point);
//}}AFX_MSG
DECLARE_MESSAGE_MAP()
};

#endif