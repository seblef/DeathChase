
#ifndef _IREFERENCECOUNTER_H_

#define _IREFERENCECOUNTER_H_

#include "../MyAssert.h"

namespace Core
{

class IReferenceCounter
{
	public:

		IReferenceCounter() : m_iCounter(1) {};
		virtual ~IReferenceCounter() {};

		void Grab() const { ++m_iCounter; };
		bool Drop()
		{
			--m_iCounter;

	//		MYASSERT(m_iCounter > 0,"Bad reference count",false);

			if(m_iCounter == 0)
			{
				if(Delete())
					delete this;

				return true;
			}

			return false;
		};

		int GetReferenceCount() const { return m_iCounter; };

		virtual bool Delete() { return true; };

	private:

		mutable int m_iCounter;

};

}

#endif
