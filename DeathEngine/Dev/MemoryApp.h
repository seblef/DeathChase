
#ifdef DEDEV

#ifndef _MEMORYAPP_H_
#define _MEMORYAPP_H_

#include "../GUI/Dialog.h"

namespace GUI
{
class MultiListBox;
}

namespace Dev
{

class MemoryApp : public GUI::Dialog
{
	public:

		MemoryApp();
		~MemoryApp();

		void OnCreate();

		void OnClick(const uint32 dwID);
		void OnSelect(const uint32 dwID);
		void OnTab(const uint32 dwID);

	protected:

		void RefreshAllocators();
		void RefreshMemDebug();
		const char *FormatSize(int iSize);
		const char *ToChar(int i);
		void ShowAllocators(bool bShow);

		GUI::ITabControl *m_pTab;

		GUI::IButton *m_pRefresh;
		GUI::IListBox *m_pAllocatorsList;
		GUI::IStaticText *m_pAllocUsed;
		GUI::IStaticText *m_pAllocFree;
		GUI::IStaticText *m_pAllocTotal;
		GUI::MultiListBox *m_pMemDebug;

};

}

#endif
#endif