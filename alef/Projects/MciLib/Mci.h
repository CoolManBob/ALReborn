#ifndef MCI_H
#define MCI_H

#include <string>
#include "MethodAnalyzer.h"

struct IMciEvents;

class Mci  
{
public:
	Mci(const std::string & filename, int lineNumber, std::string line);
	~Mci();

	static IMciEvents * & GetSink();
	static void Register(IMciEvents * pSink);

private:
	std::string GetLine(const std::string & filename, int lineNumber);

private:
	std::string		m_filename;
	int				m_lineNumber;	
	MethodInfo		m_methodInfo;
};

#endif // !defined(__MCI_H__)
