
#ifndef _TASKLIST_H_

#define _TASKLIST_H_

#include "ITask.h"

namespace Core
{

class TaskList
{
	private:
	
		typedef struct _Item
		{
			ITask *pTask;
			
			struct _Item *pPrev;
			struct _Item *pNext;
			
		} Item;

	public:
	
		TaskList();
		~TaskList();

		bool AddTask(ITask *pTask);
		bool InsertTask(ITask *pTask);
		ITask *GetTask(const int iIndex);
		int GetNbTasks() const { return m_iNbTasks; };
		bool RemoveTask(ITask *pTask);
		bool Purge();

	private:

		Item *m_pFirst;
		int m_iNbTasks;
};

}

#endif
