
#ifndef _IRENDERER2D_H_
#define _IRENDERER2D_H_

#include "../Core/Handles.h"
#include "../Core/Rect.h"
#include "../Core/Color.h"

class IRenderer2D
{
	public:

		IRenderer2D() {}
		virtual ~IRenderer2D() {}

		virtual void Start()=0;
		virtual void End()=0;

		virtual void DrawRectangle(int x1, int y1, int x2, int y2, const Core::Color32& cColor)=0;
		virtual void DrawRectangle(const Core::Color32& cColor, const Core::Rect& rect, const Core::Rect *clip=0)=0;
		virtual void DrawRectangle(const Core::Rect& pos,
			Core::Color32 lu, Core::Color32 ru, Core::Color32 ld, Core::Color32 rd, const Core::Rect *clip=0)=0;
		virtual void Draw2DImage(Core::HTexture hTex, int x1, int y1, int x2, int y2,
			float u1=0.0f,float v1=0.0f, float u2=1.0f, float v2=1.0f)=0;
		virtual void Draw2DImage(Core::HTexture hTex,const Core::Vector2i& vPos,
			const Core::Rect& sRect,
			const Core::Rect* pClip,
			Core::Color32 cColor,
			bool bUseTextureAlpha)=0;

		virtual void Execute()=0;

};

#endif
