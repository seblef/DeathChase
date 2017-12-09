
#ifndef _GUITASK_H_

#define _GUITASK_H_

#include "../Core/TSingleton.h"
#include "../Core/ITask.h"
#include "IEnvironment.h"
#include <vector>


namespace GUI
{

class IContextMenu;

class GUITask : public Core::ITask,
				public IEventReceiver,
				public Core::TSingleton<GUITask>
{
	public:

		GUITask(const int iPriority);
		~GUITask();

		bool Initialize();
		bool Stop();
		bool Update();

		bool OnSuspend();
		bool OnResume();

		IEnvironment *GetEnvironment() { return m_pEnvironment; };

		void Enable(const bool bEnable);
		bool IsCursorIn(const int x, const int y);

		IContextMenu *GetMainMenu();
		void ShowMenu(bool bShow);
		bool IsMenuShown() const;

		void AddMenuEventReceiver(IEventReceiver *pReceiver);
		void RemoveMenuEventReceiver(IEventReceiver *pReceiver);

		bool OnEvent(const Event& _Event);

	private:

		IEnvironment *m_pEnvironment;

		int iOldMX, iOldMY;
		bool m_bEnable;

		IContextMenu *m_pMenu;
		std::vector<IEventReceiver *> m_MenuReceivers;
};

}

#define g_GUI				GUI::GUITask::GetSingletonRef()

#endif
