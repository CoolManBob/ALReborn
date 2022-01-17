// MethodAnalyzer.h: interface for the MethodAnalyzer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(__METHOD_INFO_H__)
#define __METHOD_INFO_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include <vector>

struct MethodArgument
{
	std::string type;
	std::string name;
	std::string defaultValue;
};

typedef std::vector<MethodArgument> ArgumentVec;

struct MethodInfo
{
	std::string		returnType;
	std::string		className;
	std::string		name;
	ArgumentVec		argumentVec;
};

#endif // !defined(__METHOD_INFO_H__)
