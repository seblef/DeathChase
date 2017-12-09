
#ifndef _IENVIRONMENT_H_

#define _IENVIRONMENT_H_

#include "../Core/IReferenceCounter.h"
#include "../Core/Renderer.h"
#include "IEventReceiver.h"
#include "ISkin.h"
#include "IElement.h"

namespace GUI
{

class IFont;
class ISpriteBank;
class IScrollBar;
class IImage;
class ICheckBox;
class IListBox;
class IFileDialog;
class IColorDialog;
class IInOutFader;
class IStaticText;
class IEditBox;
class ISpinBox;
class ITabControl;
class ITab;
class IContextMenu;
class IComboBox;
class IToolBar;
class IButton;
class IKeyButton;
class IWindow;


enum MESSAGEBOXFLAGS
{
	MBF_OK=0x1,
	MBF_CANCEL=0x2,
	MBF_YES=0x4,
	MBF_NO=0x8,

	MBF_FORCEDWORD=0x7fffffff

};

class IEnvironment : public virtual Core::IReferenceCounter, public IElement
{
	public:

		IEnvironment() :
			IElement(ET_ELEMENT,0,0,0,0,Core::Rect(g_Renderer.GetViewportPos(),g_Renderer.GetViewportPos() + g_Renderer.GetViewportSize())) {};
			virtual ~IEnvironment() {};

		virtual void DrawAll()=0;

		virtual bool SetFocus(IElement *pElement)=0;
		virtual IElement *GetFocus() const=0;
		virtual bool RemoveFocus(IElement *pElement)=0;
		virtual bool HasFocus(IElement *pElement) const=0;

		virtual void Clear()=0;

		virtual bool PostEventFromUser(const Event& _Event)=0;
		virtual void SetUserEventReceiver(IEventReceiver *evr)=0;

		virtual ISkin *GetSkin() const=0;
		virtual void SetSkin(ISkin *pSkin)=0;
		virtual ISkin *LoadSkin(const char *szName)=0;

		virtual IFont *GetFont(const char *szFileName)=0;
		virtual IFont *GetBuiltInFont()=0;

		virtual ISpriteBank *GetSpriteBank(const char *szFileName)=0;
		virtual ISpriteBank *AddEmptySpriteBank(const char *szName)=0;

		virtual IElement *GetRootElement()=0;

		virtual IButton *AddButton(const Core::Rect& rect, IElement *pParent=0, IEventReceiver *pUser=0,
			uint32 dwID=INVALID_ID, const char *szText=0, const char *szToolTipText=0)=0;
		virtual IKeyButton *AddKeyButton(const Core::Rect& rect, IElement *pParent=0, IEventReceiver *pUser=0,
			uint32 dwID=INVALID_ID, const char *szText=0, const char *szToolTipText=0)=0;
		virtual IWindow *AddWindow(const Core::Rect& rect, bool bModal=false,
			const char *szText=0, IElement *pParent=0, IEventReceiver *pUser=0,uint32 dwID=INVALID_ID)=0;
		virtual IElement *AddModalScreen(IElement *pParent,IEventReceiver *pUser=0)=0;
		virtual IWindow *AddMessageBox(const char *szCaption, const char *szText=0,
			bool bModal=true, uint32 dwFlags=MBF_OK, IElement *pParent=0, IEventReceiver *pUser=0,
			uint32 dwID=INVALID_ID)=0;
		virtual IScrollBar *AddScrollBar(bool bHorizontal, const Core::Rect& rect,
			IElement *pParent=0, IEventReceiver *pUser=0, uint32 dwID=INVALID_ID)=0;
		virtual IImage *AddImage(Core::HTexture hTex, Core::Vector2i vPos,
			bool bUseAlpha=true, IElement *pParent=0, IEventReceiver *pUser=0, uint32 dwID=INVALID_ID, const char *szText=0)=0;
		virtual IImage *AddImage(const Core::Rect& rect, IElement *pParent=0,
			IEventReceiver *pUser=0, uint32 dwID=INVALID_ID, const char *szText=0)=0;
		virtual ICheckBox *AddCheckBox(bool bChecked, const Core::Rect& rect,
			IElement *pParent=0, IEventReceiver *pUser=0, uint32 dwID=INVALID_ID, const char *szText=0)=0;
		virtual IListBox *AddListBox(const Core::Rect& rect, IElement *pParent,
			IEventReceiver *pUser=0, uint32 dwID=INVALID_ID, bool bDrawBackground=false)=0;
		virtual IFileDialog *AddFileDialog(const char *szTitle=0, bool bModal=true, bool bImageDialog=false,
			IElement *pParent=0, IEventReceiver *pUser=0, uint32 dwID=INVALID_ID)=0;
		virtual IColorDialog *AddColorDialog(const char *szTitle=0, bool bModal=true,
			IElement *pParent=0, IEventReceiver *pUser=0, uint32 dwID=INVALID_ID)=0;
		virtual IStaticText *AddStaticText(const char *szText, const Core::Rect& rect,
			bool bBorder=false, bool bWordWrap=true, IElement *pParent=0,
			IEventReceiver *pUser=0, uint32 dwID=INVALID_ID, bool bFillBackground=false)=0;
		virtual IEditBox *AddEditBox(const char *szText, const Core::Rect& rect,
			bool bBorder=true, IElement *pParent=0, IEventReceiver *pUser=0, uint32 dwID=INVALID_ID)=0;
		virtual ISpinBox *AddSpinBox(const char *szText, const Core::Rect& rect,
			IElement *pParent=0, IEventReceiver *pUser=0, uint32 dwID=INVALID_ID)=0;
		virtual IInOutFader *AddInOutFader(const Core::Rect *pRect=0,
			IElement *pParent=0, IEventReceiver *pUser=0, uint32 dwID=INVALID_ID)=0;
		virtual ITabControl *AddTabControl(const Core::Rect& rect, IElement *pParent=0,
			IEventReceiver *pUser=0,
			bool bFillBackground=false, bool bBorder=true, uint32 dwID=INVALID_ID)=0;
		virtual ITab *AddTab(const Core::Rect& rect, IElement *pParent=0, 
			IEventReceiver *pUser=0, uint32 dwID=INVALID_ID)=0;
		virtual IContextMenu *AddContextMenu(const Core::Rect& rect,
			IElement *pParent=0, IEventReceiver *pUser=0, uint32 dwID=INVALID_ID)=0;
		virtual IContextMenu *AddMenu(IElement *pParent=0,IEventReceiver *pUser=0,uint32 dwID=INVALID_ID)=0;
		virtual IToolBar *AddToolBar(IElement *pParent=0, IEventReceiver *pUser=0,uint32 dwID=INVALID_ID)=0;
		virtual IComboBox *AddComboBox(const Core::Rect& rect, IElement *pParent=0,
			IEventReceiver *pUser=0,uint32 dwID=INVALID_ID)=0;

		virtual bool ReleaseElement(IElement *pElement)=0;

};

}

#endif

