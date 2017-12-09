
#ifndef _SELECTOR_H_
#define _SELECTOR_H_

#include "../../DeathEngine/Core/TSingleton.h"
#include "../../DeathEngine/Game/IObject.h"
#include "../../DeathEngine/Core/Vertex.h"
#include "../../DeathEngine/Core/Handles.h"
#include <vector>

class ICamera;


class Selector : public Core::TSingleton<Selector>
{
	public:

		class SelectedItem
		{
			public:

				SelectedItem() : fDistSq(0.0f),pObject(0) {}
				~SelectedItem() {}

				const SelectedItem& operator=(const SelectedItem& item)
				{
					fDistSq=item.fDistSq;
					pObject=item.pObject;
					return *this;
				}

				float fDistSq;
				Game::IObject *pObject;
		};

		Selector();
		~Selector();

		void EnableMapPicking(bool bEnable) { m_bMapPicking=bEnable; }

		void AddObject(Game::IObject *pObj) { m_Objects.push_back(pObj); }
		void RemoveObject(Game::IObject *pObj);

		int Pick(int x, int y, ICamera *pCam);
		std::vector<SelectedItem> *GetSelection() { return &m_Selection; }

		void ClearSelection() { m_Selection.clear(); }

		void SelectNearest();
		Game::IObject *GetSelected() { return m_Selected.pObject; }
		Game::IObject *RotateSelection();

		void DrawSelected();

		void ForceSelection(Game::IObject *pObj);

	protected:

		void CreateVertices();

		std::vector<Game::IObject *> m_Objects;
		std::vector<SelectedItem> m_Selection;

		SelectedItem m_Selected;

		bool m_bMapPicking;

		Core::HShader m_hBBoxShader;
		Core::VertexP m_pVertices[8];

};

#define g_Selector				Selector::GetSingletonRef()

#endif