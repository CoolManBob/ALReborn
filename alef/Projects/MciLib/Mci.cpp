#include "Mci.h"
#include "IMciEvents.h"
#include "MethodAnalyzer.h"
#include <fstream>

using std::string;
using std::ifstream;

Mci::Mci(const string & filename, int lineNumber, string line) :
		m_filename(filename),
		m_lineNumber(lineNumber)
{
	if (!GetSink())
		return;

	if (line.empty())
		line = GetLine(filename, lineNumber-2);
	
	m_methodInfo = MethodAnalyzer::Analyze(line);

	// verify corectness of class name using typeinfo
	GetSink()->OnEnter(m_filename, m_lineNumber, m_methodInfo);
}
	
Mci::~Mci()
{
	if (!GetSink())
		return;

	GetSink()->OnLeave(m_filename, m_lineNumber, m_methodInfo);
}

IMciEvents * & Mci::GetSink() 
{ 
	static IMciEvents * pSink = 0; 
	return pSink; 
}

void Mci::Register(IMciEvents * pSink) 
{ 
	GetSink() = pSink; 
}


string Mci::GetLine(const string & filename, int lineNumber)
{
	ifstream f;
	f.open(filename.c_str());
	const int BUFF_SIZE = 1024;
	char buff[BUFF_SIZE];
	for (int i = 0; i < lineNumber; i++)
		f.getline(buff, BUFF_SIZE);

	return string(buff);
}
