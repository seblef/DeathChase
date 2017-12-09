
#ifndef _ENVIRONMENT_H_

#define _ENVIRONMENT_H_

#include "IEnvironment.h"
#include "IElement.h"
#include <vector>
#include "../Core/TMemoryAllocator.h"

namespace GUI
{

class Environment : public IEnvironment //, public IElement
{
	public:

		Environment();
		virtual ~Environment();

		virtual void DrawAll();

		virtual bool PostEventFromUser(const Event& _Event);
		virtual void SetUserEventReceiver(IEventReceiver *pEvr);

		virtual void Clear();

		virtual bool OnEvent(const Event& _Event);

		virtual ISkin *GetSkin() const;
		virtual void SetSkin(ISkin *pSkin);
		virtual ISkin *LoadSkin(const char *szName);

		virtual IFont *GetFont(const char *szFileName);
		
		virtual ISpriteBank *GetSpriteBank(const char *szFileName);
		virtual ISpriteBank *AddEmptySpriteBank(const char *szName);

		virtual IButton *AddButton(const Core::Rect& rect, IElement *pParent=0,
			IEventReceiver *pUser=0, uint32 dwID=INVALID_ID, const char *szText=0, const char *szToolTipText=0);
		virtual IKeyButton *AddKeyButton(const Core::Rect& rect, IElement *pParent=0,
			IEventReceiver *pUser=0, uint32 dwID=INVALID_ID, const char *szText=0, const char *szToolTipText=0);
		virtual IWindow *AddWindow(const Core::Rect& rect, bool bModal=false,
			const char *szText=0, IElement *pParent=0, IEventReceiver *pUser=0, uint32 dwID=INVALID_ID);
		virtual IElement *AddModalScreen(IElement *pParent, IEventReceiver *pUser=0);
		virtual IWindow *AddMessageBox(const char *szCaption, const char *szText=0,
			bool bModal=true, uint32 dwFlags=MBF_OK, IElement *pParent=0,
			IEventReceiver *pUser=0, uint32 dwID=INVALID_ID);
		virtual IScrollBar *AddScrollBar(bool bHorizontal, const Core::Rect& rect,
			IElement *pParent=0, IEventReceiver *pUser=0,uint32 dwID=INVALID_ID);
		virtual IImage *AddImage(Core::HTexture hTex, Core::Vector2i vPos, bool bUseAlpha=true,
			IElement *pParent=0, IEventReceiver *pUser=0, uint32 dwID=INVALID_ID,const char *szText=0);
		virtual IImage *AddImage(const Core::Rect& rect, IElement *pParent,
			IEventReceiver *pUser=0, uint32 dwID=INVALID_ID, const char *szText=0);
		virtual ICheckBox *AddCheckBox(bool bChecked, const Core::Rect& rect,
			IElement *pParent=0, IEventReceiver *pUser=0, uint32 dwID=INVALID_ID, const char *szText=0);
		virtual IListBox *AddListBox(const Core::Rect& rect, IElement *pParent,
			IEventReceiver *pUser=0, uint32 dwID=INVALID_ID, bool bDrawBackground=false);
		virtual IFileDialog *AddFileDialog(const char *szTitle, bool bModal=true, bool bImageDialog=false,
			IElement *pParent=0, IEventReceiver *pUser=0, uint32 dwID=INVALID_ID);
		virtual IColorDialog *AddColorDialog(const char *szTitle, bool bModal=true,
			IElement *pParent=0, IEventReceiver *pUser=0, uint32 dwID=INVALID_ID);
		virtual IStaticText *AddStaticText(const char *szText, const Core::Rect& rect,
			bool bBorder=false, bool bWordWrap=true, IElement *pParent=0,
			IEventReceiver *pUser=0, uint32 dwID=INVALID_ID, bool bDrawBackground=false);
		virtual IEditBox *AddEditBox(const char *szText, const Core::Rect& rect,
			bool bBorder=false, IElement *pParent=0, IEventReceiver *pUser=0, uint32 dwID=INVALID_ID);
		virtual ISpinBox *AddSpinBox(const char *szText, const Core::Rect& rect,
			IElement *pParent=0, IEventReceiver *pUser=0, uint32 dwID=INVALID_ID);
		virtual ITabControl *AddTabControl(const Core::Rect& rect, IElement *pParent=0,
			IEventReceiver *pUser=0, bool bFillBackground=false, bool bBorder=true, uint32 dwID=INVALID_ID);
		virtual ITab *AddTab(const Core::Rect& rect, IElement *pParent=0,
			IEventReceiver *pUser=0, uint32 dwID=INVALID_ID);
		virtual IContextMenu *AddContextMenu(const Core::Rect& rect, IElement *pParent,
			IEventReceiver *pUser=0, uint32 dwID=INVALID_ID);
		virtual IContextMenu *AddMenu(IElement *pParent=0, IEventReceiver *pUser=0, uint32 dwID=INVALID_ID);
		virtual IToolBar *AddToolBar(IElement *pParent=0, IEventReceiver *pUser=0, uint32 dwID=INVALID_ID);
		virtual IComboBox *AddComboBox(const Core::Rect& rect, IElement *pParent,
			IEventReceiver *pUser=0, uint32 dwID=INVALID_ID);

		virtual bool ReleaseElement(IElement *pElement);

		virtual bool SetFocus(IElement *pElement);
		virtual bool RemoveFocus(IElement *pElement);
		virtual bool HasFocus(IElement *pElement) const;
		virtual IElement *GetFocus() const;

		virtual IFont *GetBuiltInFont();

		virtual IInOutFader *AddInOutFader(const Core::Rect *rect=0, IElement *pParent=0,
			 IEventReceiver *pUser=0, uint32 dwID=INVALID_ID);
		virtual IElement *GetRootElement();

		virtual void OnPostRender(uint32 dwTime);

	private:

		IElement *GetNextElement(bool bReverse=false, bool bGroup=false);
		void UpdateHoveredElement(Core::Vector2i vPos);

		struct SFont
		{
			std::string strFileName;
			IFont *pFont;
		};

		struct SSpriteBank
		{
			std::string strFileName;
			ISpriteBank *pBank;
		};

		struct SToolTip
		{
			uint32 dwLastTime;
			uint32 dwLaunchTime;
			IStaticText *pElement;
		};

		SToolTip m_ToolTip;
		std::vector<SFont> m_Fonts;
		std::vector<SSpriteBank> m_Banks;

		IElement *m_pHovered;
		IElement *m_pFocus;

		Core::Vector2i m_vLastHoveredPos;
		ISkin *m_pCurrentSkin;
		IEventReceiver *m_pUserReceiver;

		Core::TMemoryAllocator<IElement> *m_pAllocators[ET_COUNT];
};

}

#endif