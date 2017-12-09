
#ifndef _VERTEX_H_

#define _VERTEX_H_

#include "Vector.h"
#include "Color.h"

namespace Core
{


typedef enum _VertexType
{
	VX_PNDTT, //		pos-norm-diff-2x tex
	VX_PNTT, //			pos-norm-2x tex
	VX_PNDT, //			pos-norm-diff-tex
	VX_PNT, //			pos-norm-tex
	VX_PDTT, //			pos-diff-2x tex
	VX_PTT, //			pos-2x tex
	VX_PDT, //			pos-diff-tex
	VX_PT, //			pos-tex,
	VX_PD, //			pos-diff
	VX_2DPDT, //		2d-pos-diff-tex
	VX_2DPT, //			2d-pos-tex
	VX_2DPD, //			2d-pos-diff

	VX_COUNT

} VertexType;


class VertexPNDTT
{
	public:

		VertexPNDTT() {};
		VertexPNDTT(const VertexPNDTT& v) : vPos(v.vPos),vNorm(v.vNorm),
			cColor(v.cColor),vUV0(v.vUV0),vUV1(v.vUV1) {};
		VertexPNDTT(const Vector3& pos, const Vector3& norm,
			const Color32& color,const Vector2& uv0, const Vector2& uv1) :
			vPos(pos),vNorm(norm),cColor(color),vUV0(uv0),vUV1(uv1) {};
		~VertexPNDTT() {};

		const VertexPNDTT& operator=(const VertexPNDTT& v)
		{ vPos=v.vPos; vNorm=v.vNorm; cColor=v.cColor; vUV0=v.vUV0; vUV1=v.vUV1; return *this; };

		Vector3 vPos;
		Vector3 vNorm;
		Color32 cColor;
		Vector2 vUV0;
		Vector2 vUV1;

};

class VertexPNTT
{
	public:

		VertexPNTT() {};
		VertexPNTT(const VertexPNTT& v) : vPos(v.vPos),vNorm(v.vNorm),
			vUV0(v.vUV0),vUV1(v.vUV1) {};
		VertexPNTT(const Vector3& pos, const Vector3& norm, const Vector2& uv0, const Vector2& uv1) :
			vPos(pos),vNorm(norm),vUV0(uv0),vUV1(uv1) {};
		~VertexPNTT() {};

		const VertexPNTT& operator=(const VertexPNTT& v)
		{ vPos=v.vPos; vNorm=v.vNorm; vUV0=v.vUV0; vUV1=v.vUV1; return *this; };

		Vector3 vPos;
		Vector3 vNorm;
		Vector2 vUV0;
		Vector2 vUV1;

};

class VertexPNDT
{
	public:

		VertexPNDT() {};
		VertexPNDT(const VertexPNDT& v) : vPos(v.vPos),vNorm(v.vNorm),cColor(v.cColor),vUV(v.vUV) {};
		VertexPNDT(const Vector3& pos, const Vector3& norm, const Color32& color, const Vector2& uv) :
			vPos(pos),vNorm(norm),cColor(color),vUV(uv) {};
		~VertexPNDT() {};

		const VertexPNDT& operator=(const VertexPNDT& v)
		{ vPos=v.vPos; vNorm=v.vNorm; cColor=v.cColor; vUV=v.vUV; return *this; };

		Vector3 vPos;
		Vector3 vNorm;
		Color32 cColor;
		Vector2 vUV;

};

class VertexPNT
{
	public:

		VertexPNT() {};
		VertexPNT(const VertexPNT& v) : vPos(v.vPos),vNorm(v.vNorm),vUV(v.vUV) {};
		VertexPNT(const Vector3& pos, const Vector3& norm, const Vector2& uv) :
			vPos(pos),vNorm(norm),vUV(uv) {};
		~VertexPNT() {};

		const VertexPNT& operator=(const VertexPNT& v)
		{ vPos=v.vPos; vNorm=v.vNorm; vUV=v.vUV; return *this; };

		Vector3 vPos;
		Vector3 vNorm;
		Vector2 vUV;

};

class VertexPDTT
{
	public:

		VertexPDTT() {};
		VertexPDTT(const VertexPDTT& v) : vPos(v.vPos),cColor(v.cColor),vUV0(v.vUV0),vUV1(v.vUV1) {};
		VertexPDTT(const Vector3& pos, const Color32& color, const Vector2& uv0, const Vector2& uv1) :
			vPos(pos),cColor(color),vUV0(uv0),vUV1(uv1) {};
		~VertexPDTT() {};

		const VertexPDTT& operator=(const VertexPDTT& v)
		{ vPos=v.vPos; cColor=v.cColor; vUV0=v.vUV0; vUV1=v.vUV1; return *this; };

		Vector3 vPos;
		Color32 cColor;
		Vector2 vUV0;
		Vector2 vUV1;

};

class VertexPTT
{
	public:

		VertexPTT() {};
		VertexPTT(const VertexPTT& v) : vPos(v.vPos), vUV0(v.vUV0),vUV1(v.vUV1) {};
		VertexPTT(const Vector3& pos, const Vector2& uv0, const Vector2& uv1) :
			vPos(pos),vUV0(uv0),vUV1(uv1) {};
		~VertexPTT() {};

		const VertexPTT& operator=(const VertexPTT& v)
		{ vPos=v.vPos; vUV0=v.vUV0; vUV1=v.vUV1; return *this; };

		Vector3 vPos;
		Vector2 vUV0;
		Vector2 vUV1;

};

class VertexPDT
{
	public:

		VertexPDT() {};
		VertexPDT(const VertexPDT& v) : vPos(v.vPos), cColor(v.cColor),vUV(v.vUV) {};
		VertexPDT(const Vector3& pos, const Color32& color, const Vector2& uv) :
			vPos(pos),cColor(color),vUV(uv) {};
		~VertexPDT() {};

		const VertexPDT& operator=(const VertexPDT& v)
		{ vPos=v.vPos; cColor=v.cColor; vUV=v.vUV; return *this; };

		Vector3 vPos;
		Color32 cColor;
		Vector2 vUV;

};

class VertexPT
{
	public:

		VertexPT() {};
		VertexPT(const VertexPT& v) : vPos(v.vPos),vUV(v.vUV) {};
		VertexPT(const Vector3& pos, const Vector2& uv) : vPos(pos),vUV(uv) {};
		~VertexPT() {};

		const VertexPT& operator=(const VertexPT& v) { vPos=v.vPos; vUV=v.vUV; return *this; };

		Vector3 vPos;
		Vector2 vUV;

};

class VertexPD
{
	public:

		VertexPD() {};
		VertexPD(const VertexPD& v) : vPos(v.vPos), cColor(v.cColor) {};
		VertexPD(const Vector3& pos, const Color32& color) : vPos(pos),cColor(color) {};
		~VertexPD() {};

		const VertexPD& operator=(const VertexPD& v) { vPos=v.vPos; cColor=v.cColor; return *this; };

		Vector3 vPos;
		Color32 cColor;

};

class Vertex2DPDT
{
	public:

		Vertex2DPDT() {};
		Vertex2DPDT(const Vertex2DPDT& v) : vPos(v.vPos),rhw(v.rhw),cColor(v.cColor),vUV(v.vUV) {};
		Vertex2DPDT(const Vector3& pos, const float _rhw, const Color32& color, const Vector2& uv) :
			vPos(pos),rhw(_rhw),cColor(color),vUV(uv) {};
		~Vertex2DPDT() {};

		const Vertex2DPDT& operator=(const Vertex2DPDT& v)
		{ vPos=v.vPos; rhw=v.rhw; cColor=v.cColor; vUV=v.vUV; return *this; };

		Vector3 vPos;
		float rhw;
		Color32 cColor;
		Vector2 vUV;

};

class Vertex2DPT
{
	public:

		Vertex2DPT() {};
		Vertex2DPT(const Vertex2DPT& v) : vPos(v.vPos),rhw(v.rhw),vUV(v.vUV) {};
		Vertex2DPT(const Vector3& pos, const float _rhw, const Vector2& uv) :
			vPos(pos),rhw(_rhw),vUV(uv) {};
		~Vertex2DPT() {};

		const Vertex2DPT& operator=(const Vertex2DPDT& v)
		{ vPos=v.vPos; rhw=v.rhw; vUV=v.vUV; return *this; };

		Vector3 vPos;
		float rhw;
		Vector2 vUV;

};

class Vertex2DPD
{
	public:

		Vertex2DPD() {};
		Vertex2DPD(const Vertex2DPD& v) : vPos(v.vPos),rhw(v.rhw),cColor(v.cColor) {};
		Vertex2DPD(const Vector3& pos, const float _rhw, const Color32& color) :
			vPos(pos),rhw(_rhw),cColor(color) {};
		~Vertex2DPD() {};

		const Vertex2DPD& operator=(const Vertex2DPD& v)
		{ vPos=v.vPos; rhw=v.rhw; cColor=v.cColor; return *this; };

		Vector3 vPos;
		float rhw;
		Color32 cColor;

};

extern int VertexSizeTable[VX_COUNT];

}


#endif