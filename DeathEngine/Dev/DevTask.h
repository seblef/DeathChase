
#ifdef DEDEV

#ifndef _DEVTASK_H_
#define _DEVTASK_H_

#include "../Core/ITask.h"
#include "../Core/TSingleton.h"
#include "../GUI/IEventReceiver.h"

namespace Dev
{

class ConsoleApp;
class MemoryApp;

class DevTask : public Core::ITask,
				public GUI::IEventReceiver,
				public Core::TSingleton<DevTask>
{
	public:

		DevTask(const int iPriority);
		~DevTask();

		bool Initialize();
		bool Stop();

		bool OnSuspend();
		bool OnResume();

		bool Update();

		bool OnEvent(const GUI::Event& _Event);

	private:

		ConsoleApp *m_pConsole;
		MemoryApp *m_pMemory;

};

}

#define g_Dev				Dev::DevTask::GetSingletonRef()

#endif
#endif