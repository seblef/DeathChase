
#ifndef _TREEGENERATOR_H_
#define _TREEGENERATOR_H_

#include "../Core/TSingleton.h"
#include "../Core/Handles.h"
#include "../Core/TDynamicTab.h"
#include "../Core/Vertex.h"

#include "TreeDefs.h"

#ifdef DEEXPORT
#include "../Export/ITreeGenerator.h"
#endif

namespace Game
{

#ifdef DEEXPORT
class TreeGenerator : public ITreeGenerator, public Core::TSingleton<TreeGenerator>
#else
class TreeGenerator : public Core::TSingleton<TreeGenerator>
#endif
{
	public:

		TreeGenerator() {}
		~TreeGenerator();

		bool GetTreeMesh(const TreeType _Type, Core::HMesh& hNormal, Core::HMesh& hBurned);

	protected:

		bool Generate(const TreeType _Type);

		void GeneratePinTrunc(Core::TDynamicTab<Core::VertexPNDTT> *pVertices,
			Core::TDynamicTab<uint16> *pIndices,
			float fWidth, float fHeight, float fLowHeight,
			float x=0.875f, float y=0.0f, float w=0.125f, float h=0.1f,
			float fSteps=3.0f,
			const Core::Color32 &cColor=Core::Color32(255,255,255,255));
		void GeneratePinLevel(Core::TDynamicTab<Core::VertexPNDTT> *pVertices,
			Core::TDynamicTab<uint16> *pIndices,
			float fTexU, float fTexV, float fWidth, float fHeight, float fLowHeight,
			float fTexWidth=0.125f, float fCount=5.0f, bool bDoubleSide=false, float fAngOffset=0.0f,
			const Core::Color32 &cColor=Core::Color32(255,255,255,255));
		void GeneratePalmTrunc(Core::TDynamicTab<Core::VertexPNDTT> *pVertices,
			Core::TDynamicTab<uint16> *pIndices,
			float fWidth, float fHeight, float fCount,
			float x=0.0f, float y=0.0f,
			float w=1.0f, float h=0.125f,
			const Core::Color32 &cColor=Core::Color32(255,255,255,255));
		void GeneratePalmLevel(Core::TDynamicTab<Core::VertexPNDTT> *pVertices,
			Core::TDynamicTab<uint16> *pIndices,
			float fWidth1, float fWidth2, float fHeight, float fHeight2,
			float fCount, float fTexU, float fTexV,
			const Core::Color32 &cColor=Core::Color32(255,255,255,255));

		Core::HMesh m_hMeshes[TREE_COUNT];

};

}

#define g_TreeGenerator					Game::TreeGenerator::GetSingletonRef()

#endif