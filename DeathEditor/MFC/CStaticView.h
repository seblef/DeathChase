
#ifndef _CSTATICVIEW_H_
#define _CSTATICVIEW_H_

#include "../stdafx.h"
#include "../../DeathEngine/Core/Handles.h"

class ICamera;

class CStaticMeshView : public CStatic
{
	public:

		CStaticMeshView();
		~CStaticMeshView();

		void Draw();
		ICamera *GetCamera() { return m_pCamera; }
		void SetMesh(Core::HMesh hMesh);

		afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
		afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
		afx_msg void OnMouseMove(UINT nFlags, CPoint point);
		afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
		afx_msg void OnPaint();

		DECLARE_MESSAGE_MAP()

	protected:

		ICamera *m_pCamera;
		Core::HMesh m_hMesh;

		bool m_bDragging;
		int m_iLastX,m_iLastY;
};

#endif