
#ifndef _ITREEGENERATOR_H_
#define _ITREEGENERATOR_H_

#include "../Core/Handles.h"
#include "../Game/TreeDefs.h"

class ITreeGenerator
{
	public:

		ITreeGenerator() {}
		virtual ~ITreeGenerator() {}

		virtual bool GetTreeMesh(const TreeType _Type, Core::HMesh& hNormal, Core::HMesh& hBurned)=0;
};

#endif
