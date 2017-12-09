
#ifndef _SELECTOR_H_
#define _SELECTOR_H_

#include "../DeathEngine/Core/TSingleton.h"
#include "../DeathEngine/Core/Vertex.h"
#include "SObject.h"
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
				SObject *pObject;
		};

		Selector();
		~Selector();

		void RegisterObject(SObject *pObject) { m_Objects.push_back(pObject); }
		void RemoveObject(SObject *pObject);

		int Pick(int x, int y, ICamera *pCam);
		std::vector<SelectedItem> *GetSelection() { return &m_Selection; }

		void ClearSelection() { m_Selection.clear(); }

		void SelectNearest();
		SObject *GetSelected() { return m_Selected.pObject; }
		SObject *RotateSelection();

		void DrawSelected();

		void ForceSelection(SObject *pObj);

	protected:

		void CreateVertices();

		std::vector<SObject *> m_Objects;
		std::vector<SelectedItem> m_Selection;

		SelectedItem m_Selected;

		bool m_bMapPicking;

		Core::HShader m_hBBoxShader;
		Core::Vector3 m_pVertices[8];

};

#define g_Selector				Selector::GetSingletonRef()

#endif