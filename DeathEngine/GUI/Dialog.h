
#ifndef _DIALOG_H_

#define _DIALOG_H_

#include "IEnvironment.h"
#include "../Core/ScriptFile.h"
#include <vector>

namespace GUI
{

class Dialog : public IEventReceiver
{
	public:

		Dialog(bool bModal=false);
		~Dialog();

		bool Create(const int x,
					const int y,
					const int width,
					const int height,
					const char *szCaption,
					const bool bExecute=false,
					const uint32 dwID=INVALID_ID,
					bool bOnCreateMsg=true);
		bool LoadTemplate(const char *szFileName);
		bool Destroy();

		void SetCaption(const char *szCaption);

		void Show(const bool bShow);
		void Show();
		void Hide();

		bool IsShown() const { return m_bShow; };

		virtual void OnShow() {};
		virtual void OnHide() {};
		virtual void OnCreate() {};
		virtual void OnDestroy() {};

		virtual void OnMinimize() {};
		virtual void OnRestore() {};

		virtual void OnTimer() {};

		virtual void OnMouseMove(const int x, const int y) {};
		virtual void OnMouseDown(const int x, const int y) {};
		virtual void OnMouseUp(const int x, const int y) {};
		virtual void OnClick(const uint32 dwID) {};
		virtual void OnDblClick(const uint32 dwID) {};

		virtual void OnSetFocus() {};
		virtual void OnKillFocus() {};

		virtual void OnMove() {};

		virtual void OnScroll(const uint32 dwID) {};
		virtual void OnCheck(const uint32 dwID) {};
		virtual void OnSelect(const uint32 dwID) {};
		virtual void OnMenu(const uint32 dwID) {};
		virtual void OnEditBoxEnter(const uint32 dwID) {};
		virtual void OnTab(const uint32 dwID) {};
		virtual void OnSpinBox(const uint32 dwID) {};
		virtual void OnKeySelected(const uint32 dwID) {}

		bool OnEvent(const Event& e);

		IElement *GetElementByID(const uint32 dwID);
		void RemoveControl(IElement *pControl);
		void AddControl(IElement *pControl, bool bAddAsChild=false);

		char *GetName() { return m_szName; };

	protected:

		bool ParseSize(Core::ScriptFile *pFile, Core::Rect& r);
		void ParseAlignment(Core::ScriptFile *pFile, IElement *pElement);
		bool CheckBrace(Core::ScriptFile *pFile);
		bool ParseItems(Core::ScriptFile *pFile, std::vector<std::string> *pItems);

		bool ParseButton(Core::ScriptFile *pFile, Core::Rect& r);
		bool ParseKeyButton(Core::ScriptFile *pFile, Core::Rect& r);
		bool ParseCheckBox(Core::ScriptFile *pFile, Core::Rect& r);
		bool ParseComboBox(Core::ScriptFile *pFile, Core::Rect& r);
		bool ParseEditBox(Core::ScriptFile *pFile, Core::Rect& r);
		bool ParseImage(Core::ScriptFile *pFile, Core::Rect& r);
		bool ParseListBox(Core::ScriptFile *pFile, Core::Rect& r);
		bool ParseMenu(Core::ScriptFile *pFile);
		bool ParseScrollBar(Core::ScriptFile *pFile, Core::Rect& r);
		bool ParseSpinBox(Core::ScriptFile *pFile, Core::Rect& r);
		bool ParseStaticText(Core::ScriptFile *pFile, Core::Rect& r);
		bool ParseTab(Core::ScriptFile *pFile, Core::Rect& r);

		bool ParseToolBar(const char *szFileName, IToolBar *pBar, ISpriteBank *pBank);

		IWindow *m_pWindow;
		bool m_bModal;
		IEnvironment *m_pEnvironment;

		bool m_bShow;
		bool m_bMenuCreated;

		char m_szName[128];

		std::vector<IElement *> m_Controls;
};

}

#define GetButton(x)		((GUI::IButton *)GetElementByID(x))
#define GetKeyButton(x)		((GUI::IKeyButton *)GetElementByID(x))
#define GetCheckBox(x)		((GUI::ICheckBox *)GetElementByID(x))
#define GetComboBox(x)		((GUI::IComboBox *)GetElementByID(x))
#define GetEditBox(x)		((GUI::IEditBox *)GetElementByID(x))
#define GetImage(x)			((GUI::IImage *)GetElementByID(x))
#define GetListBox(x)		((GUI::IListBox *)GetElementByID(x))
#define GetScrollBar(x)		((GUI::IScrollBar *)GetElementByID(x))
#define GetSpinBox(x)		((GUI::ISpinBox *)GetElementByID(x))
#define GetStatic(x)		((GUI::IStaticText *)GetElementByID(x))
#define GetTabControl(x)	((GUI::ITabControl *)GetElementByID(x))

#endif
