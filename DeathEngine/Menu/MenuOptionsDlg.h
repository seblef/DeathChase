
#ifndef _MENUOPTIONSDLG_H_
#define _MENUOPTIONSDLG_H_

#include "../GUI/Dialog.h"

namespace Menu
{

class MenuOptionsDlg : public GUI::Dialog
{
	public:

		MenuOptionsDlg();
		~MenuOptionsDlg();

		void OnCreate();
		void OnClick(const uint32 dwID);
		void OnTab(const uint32 dwID);
		void OnCheck(const uint32 dwID);

		bool IsFinished() const { return m_bFinished; }

	private:

		void ShowVideo(bool bShow);
		void ShowKeys(bool bShow);
		void ApplyChanges();
		void FillAntialiasCombo(bool bWindowed);
		void FillResCombo(bool bWindowed);

		bool m_bFinished;
};

}

#endif