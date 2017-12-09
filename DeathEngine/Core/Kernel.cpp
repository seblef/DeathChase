
#include "Kernel.h"

namespace Core
{

Kernel::Kernel() : m_dwCurrentFrame(0),m_bKillTasks(false)
{}

Kernel::~Kernel()
{}

bool Kernel::AddTask(ITask *pTask)
{
	MYASSERT(pTask,"Bad task pointer!",true);

	if(!pTask->Initialize())
	{
		g_Output->AddMsg("x Fail initializing task %s\n",pTask->GetTaskName());
		return false;
	}

	if(!m_ExecList.InsertTask(pTask))
	{
		g_Output->AddMsg("x Failed inserting task %s\n",pTask->GetTaskName());
		ASSERTRETURN(pTask->Stop(),true,"Failed stoping task");
		return false;
	}

	g_Output->AddMsg("! %s task successfully added to kernel!\n",pTask->GetTaskName());

	return true;
}

bool Kernel::RemoveTask(ITask *pTask)
{
	MYASSERT(pTask,"Bad task pointer",true);

	if(!pTask->Stop())
		g_Output->AddMsg("x Erreur lors de l'arret de la tache %s\n",pTask->GetTaskName());

	if(!m_ExecList.RemoveTask(pTask))
	{
		if(!m_PauseList.RemoveTask(pTask))
		{
			g_Output->AddMsg("x Failed removing task %s\n",pTask->GetTaskName());
			return false;
		}
	}

	return true;
}

bool Kernel::SuspendTask(ITask *pTask)
{
	MYASSERT(pTask,"Bad task pointer",true);

	if(!m_ExecList.RemoveTask(pTask))
	{
		g_Output->AddMsg("x Failed suspended task %s\n",pTask->GetTaskName());
		return false;
	}

	m_PauseList.AddTask(pTask);
	ASSERTRETURN(pTask->OnSuspend(),true,"Task suspension");

	return true;
}



bool Kernel::SuspendAllTasks()
{
	int iActiveCount=m_ExecList.GetNbTasks();

	if(iActiveCount==0)	return true;
	int i;

	for(i=0;i<iActiveCount;i++)
	{
		ITask *ttemp=m_ExecList.GetTask(i);

		if(ttemp->OnSuspend())
		{
			ASSERTRETURN(m_ExecList.RemoveTask(ttemp),true,"Task suspension");
			ASSERTRETURN(m_PauseList.AddTask(ttemp),true,"Task suspension");

			iActiveCount--;
			i--;
		}
		else
			g_Output->AddMsg("o Unable to suspend task %s\n",ttemp->GetTaskName());
	}

	g_Output->AddMsg("o All tasks successfuly suspended\n");

	return true;
}

bool Kernel::ResumeAllTasks()
{
	const int iInactiveCount=m_PauseList.GetNbTasks();

	if(iInactiveCount==0) return true;
	int i;

	for(i=0;i<iInactiveCount;i++)
	{
		ITask *ttemp=m_PauseList.GetTask(i);
		ASSERTRETURN(ttemp->OnResume(),true,"Task resume");
		m_ExecList.InsertTask(ttemp);
	}

	m_PauseList.Purge();
	g_Output->AddMsg("o All tasks successfuly resumed\n");

	return true;
}

bool Kernel::ResumeTask(ITask *pTask)
{
	if(!m_PauseList.RemoveTask(pTask))
	{
		g_Output->AddMsg("x Failed resuming task %s\n",pTask->GetTaskName());
		return false;
	}

	m_ExecList.InsertTask(pTask);
	ASSERTRETURN(pTask->OnResume(),true,"Resuming task");

	return true;
}

bool Kernel::Execute()
{
	if(m_ExecList.GetNbTasks()==0)
	{
		g_Output->AddMsg("x Unable to execute kernel: no tasks\n");
		return false;
	}

	int i;
	while(m_ExecList.GetNbTasks()!=0  || m_PauseList.GetNbTasks()!=0)
	{
		for(i=0;i<m_ExecList.GetNbTasks();i++)
		{
			ITask *t=m_ExecList.GetTask(i);

			if(t->CanKill())
			{
				i--;
				ASSERTRETURN(t->Stop(),true,"Killing task");
				ASSERTRETURN(m_ExecList.RemoveTask(t),true,"Killin task");
			}
			else
			{
				ASSERTRETURN(t->Update(),true,"Updating task");
			}
		}

		if(m_bKillTasks)
		{
			KillAllTasks();
			m_bKillTasks=false;
			return true;
		}

		m_dwCurrentFrame++;
	}

	return true;
}

bool Kernel::KillAllTasks()
{
	int i;

	for(i=0;i<m_PauseList.GetNbTasks();i++)
	{
		ASSERTRETURN(m_PauseList.GetTask(i)->Stop(),true,"x Erreur a l'arret d'une tache");
	}

	m_PauseList.Purge();

	for(i=0;i<m_ExecList.GetNbTasks();i++)
		m_ExecList.GetTask(i)->Kill(true);

	return true;
}


}