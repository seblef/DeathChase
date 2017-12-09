
#ifndef _ITASK_H_

#define _ITASK_H_

namespace Core
{

class ITask
{
	public:
	      
		ITask(const int iPriority) : m_iPriority(iPriority), m_bCanKill(false) {};
        ~ITask() {};
        
        virtual bool Initialize()=0;
        virtual bool Stop()=0;
        
        virtual bool OnSuspend()=0;
        virtual bool OnResume()=0;
        
        virtual bool Update()=0;
        
        const char *GetTaskName() const { return m_szName; };
		bool CanKill() const { return m_bCanKill; };
		int GetPriority() const { return m_iPriority; };
		void Kill(const bool bKill) { m_bCanKill=bKill; };
        
    protected:
    
    	char m_szName[32];
        bool m_bCanKill;
        int m_iPriority;
    	
};

}

#endif