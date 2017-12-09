
#include "TaskList.h"
#include "../MyAssert.h"

namespace Core
{

TaskList::TaskList() : m_pFirst(0),m_iNbTasks(0)
{}

TaskList::~TaskList()
{
	Purge();
}

bool TaskList::AddTask(ITask *pTask)
{
	MYASSERT(pTask,"Bad task pointer!",false);

	if(!m_pFirst)
	{
		if(!(m_pFirst=new Item))
			return false;

		m_pFirst->pNext=0;
		m_pFirst->pPrev=0;
		m_pFirst->pTask=pTask;
	}
	else
	{
		Item *l=m_pFirst;

		while(l->pNext)
			l=l->pNext;

		if(!(l->pNext=new Item))
			return false;

		l->pNext->pPrev=l;
		l=l->pNext;
		l->pNext=0;
		l->pTask=pTask;
	}

	m_iNbTasks++;

	return true;
}

bool TaskList::InsertTask(ITask *pTask)
{
	MYASSERT(pTask,"Bad task pointer!",false);

	if(!m_pFirst)
		return AddTask(pTask);

	if(pTask->GetPriority() < m_pFirst->pTask->GetPriority())
	{
		Item *l;

		if(!(l=new Item))
			return false;

		l->pPrev=0;
		l->pNext=m_pFirst;
		l->pNext->pPrev=l;
		l->pTask=pTask;
		m_pFirst=l;

		m_iNbTasks++;
		return true;
	}

	if((!m_pFirst->pNext) && (pTask->GetPriority() >= m_pFirst->pTask->GetPriority()))
		return AddTask(pTask);

	Item *l=m_pFirst->pNext;

	while(l)
	{
		if(pTask->GetPriority() < l->pTask->GetPriority())
		{
			Item *ltemp;

			if(!(ltemp=new Item))
				return false;

			ltemp->pTask=pTask;
			ltemp->pPrev=l->pPrev;
			ltemp->pNext=l;
			l->pPrev->pNext=ltemp;
			l->pPrev=ltemp;

			m_iNbTasks++;
			return true;
		}
		
		l=l->pNext;
	}

	return AddTask(pTask);
}

bool TaskList::RemoveTask(ITask *pTask)
{
	MYASSERT(pTask,"Bad task pointer!",false);

	if(!m_pFirst)
		return false;

	if(m_pFirst->pTask==pTask)
	{
		if(!m_pFirst->pNext)
		{
			delete m_pFirst;
			m_pFirst=0;
		}
		else
		{
			Item *ltemp=m_pFirst;

			m_pFirst=m_pFirst->pNext;
			m_pFirst->pPrev=0;

			delete ltemp;
		}

		m_iNbTasks--;
		return true;
	}

	Item *l=m_pFirst;

	while(l)
	{
		if(l->pTask==pTask)
		{
			l->pPrev->pNext=l->pNext;

			if(l->pNext)
				l->pNext->pPrev=l->pPrev;

			delete l;

			m_iNbTasks--;
			return true;
		}

		l=l->pNext;
	}

	return false;
}

ITask *TaskList::GetTask(const int iIndex)
{
	MYASSERT(iIndex < m_iNbTasks,"Bad task index!",false);

	int i;
	Item *l=m_pFirst;

	for(i=0;i<iIndex;i++)
		l=l->pNext;

	return l->pTask;
}

bool TaskList::Purge()
{
	if(!m_pFirst)
		return true;

	if(!m_pFirst->pNext)
	{
		delete m_pFirst;
		return true;
	}

	Item *l=m_pFirst;

	while(l->pNext)
	{
		l=l->pNext;
		delete l->pPrev;
	}

	delete l;

	m_pFirst=0;
	m_iNbTasks=0;

	return true;
}

}
