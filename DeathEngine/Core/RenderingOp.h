
#ifndef _RENDERINGOP_H_
#define _RENDERINGOP_H_

#include "../Common.h"

namespace Core
{

enum RenderingCommand
{
	RC_NOEXEC=0,
	RC_EXECCOMMANDBLOCK,
	RC_SETSTATEBLOCK,
	RC_SETRENDERSTATE,
	RC_SETTEXTURE,
	RC_SETSHADER,
	RC_SETTRANSFORMATION,
	RC_SETVERTEXSHADERCONSTANT,
	RC_SETVERTEXDECLARATION,
	RC_CLEARBUFFER,
	RC_SETVERTEXBUFFER,
	RC_SETINDEXBUFFER,
	RC_RENDERSTATICBUFFERS,
	RC_RENDERDYNAMICBUFFERS,
	RC_SETMATERIAL,

	RC_COUNT
};

const char * const szRenderingOpNames[]=
{
	"None",
	"ExecCommandBlock",
	"SetStateBlock",
	"SetRenderState",
	"SetTexture",
	"SetShader",
	"SetTransformation",
	"SetVertexShaderConstant",
	"SetVertexDeclaration",
	"ClearBuffer",
	"SetVertexBuffer",
	"SetIndexBuffer",
	"RenderStaticBuffers",
	"RenderDynamicBuffers",
	"SetMaterial"

};

class RenderingOp
{
	public:

		RenderingOp() : iCmd(RC_NOEXEC),pParam(0),dwParam1(0),dwParam2(0) {}
		RenderingOp(RenderingCommand _iCmd, void *_pParam=0, uint32 _dwParam1=0, uint32 _dwParam2=0) :
			iCmd(_iCmd), pParam(_pParam), dwParam1(_dwParam1), dwParam2(_dwParam2) {}
		RenderingOp(const RenderingOp& op) : iCmd(op.iCmd), pParam(op.pParam),dwParam1(op.dwParam1),dwParam2(op.dwParam2) {}
		~RenderingOp() {}

		bool IsValid() const { return iCmd > RC_NOEXEC && iCmd < RC_COUNT; }

		RenderingCommand iCmd;
		void *pParam;
		uint32 dwParam1,dwParam2;

};

}

#endif