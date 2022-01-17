#ifndef __METHOD_ANALYZER_H__
#define __METHOD_ANALYZER_H__

#include "MethodInfo.h"

class MethodAnalyzer  
{
public:
	static MethodInfo Analyze(std::string line);

private:
	static void				SplitReturnTypeFromClassName(std::string s, std::string & returnType, std::string & className);
	static void				SplitMethodNameFromArguments(std::string s, std::string & methodName, std::string & arguments);
	static std::string		Trim(std::string s);
	static void				PopulateArgumentVec(ArgumentVec & av, std::string s);
	static MethodArgument	CreateMethodArgument(std::string s);
};

#endif
