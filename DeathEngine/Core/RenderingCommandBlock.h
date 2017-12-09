
#ifndef _RENDERINGCOMMANDBLOCK_H_
#define _RENDERINGCOMMANDBLOCK_H_

#include "RenderingOp.h"
#include "TMemoryAllocator.h"
#include <vector>

class IRenderingCommandBlock {};

namespace Core
{

class RenderingCommandBlock : public IRenderingCommandBlock
{
	public:

		RenderingCommandBlock() {}
		~RenderingCommandBlock() {}

		int GetCommandCount() const { return m_Cmds.size(); }
		bool IsValid() const
		{
			std::vector<RenderingOp>::const_iterator i;
			for(i=m_Cmds.begin();i!=m_Cmds.end();i++)
				if(!i->IsValid())
					return false;

			return true;
		}

		void AddOp(const RenderingOp& op) { m_Cmds.push_back(op); }
		void Clear() { m_Cmds.clear(); }
		
		const std::vector<RenderingOp> *GetOps() const { return &m_Cmds; }

		static RenderingCommandBlock *GetNew();
		static void Delete(RenderingCommandBlock *pBlock);
		static void InitAllocator();

	protected:

		std::vector<RenderingOp> m_Cmds;

		static TMemoryAllocator<RenderingCommandBlock> *m_spAllocator;

};

}

#endif