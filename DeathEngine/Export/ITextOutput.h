
#ifndef _ITEXTOUTPUT_H_
#define _ITEXTOUTPUT_H_

typedef void (*TextOutputFct)(const char *);

class ITextOutput
{
	public:

		ITextOutput() {}
		virtual ~ITextOutput() {}

		virtual void AddMsg(const char *szMsg,...)=0;
		virtual void SetOutputRedirect(TextOutputFct pFunc)=0;

};

#endif
