
#ifndef _KERNEL_H_

#define _KERNEL_H_

#include "TSingleton.h"
#include "TaskList.h"

namespace Core
{

class Kernel : public TSingleton<Core::Kernel>
{
	public:

		Kernel();
		~Kernel();

		bool AddTask(ITask *pTask);
		bool RemoveTask(ITask *pTask);

		bool SuspendTask(ITask *pTask);
		bool ResumeTask(ITask *pTask);
	
		bool SuspendAllTasks();
		bool ResumeAllTasks();

		bool KillAllTasks();

		bool Execute();

		unsigned long int GetNbFrames() const { return m_dwCurrentFrame; };

	private:

		TaskList m_ExecList;
		TaskList m_PauseList;

		bool m_bKillTasks;
		unsigned long int m_dwCurrentFrame;

};

}

#define g_Kernel				Core::Kernel::GetSingletonRef()

#endif
