#include "MethodAnalyzer.h"
#include "Tokenizer.h"
#include <algorithm>

using std::string;
using std::find;

MethodInfo MethodAnalyzer::Analyze(string line)
{
	MethodInfo m;

	if (line.empty())
		return m;

	// Split return type + class name from method name + arguments
	Tokenizer t1(line, ":");

	string lhs = t1.GetNextToken(); // return type + class name
	string rhs = t1.GetNextToken(); // method name + arguments
	string sReconstruction = lhs + string("::") + rhs;

	// Make sure everything is OK
	if (lhs.empty() || rhs.empty() || line != sReconstruction)
		return m;

	// Split return type from class name
	SplitReturnTypeFromClassName(lhs, m.returnType, m.className);
	
	string sArguments;
	SplitMethodNameFromArguments(rhs, m.name, sArguments);

	PopulateArgumentVec(m.argumentVec, sArguments);

	return m;
}

void MethodAnalyzer::SplitReturnTypeFromClassName(string s, string & returnType, string & clasname)
{
	Tokenizer t(s, " ");
	returnType = t.GetNextToken();
	clasname  = t.GetNextToken();
}

void MethodAnalyzer::SplitMethodNameFromArguments(string s, string & sMethodName, string & sArguments)
{
	Tokenizer t(s, "()");
	sMethodName = Trim(t.GetNextToken());
	sArguments  = t.GetNextToken();	
}

string MethodAnalyzer::Trim(string s)
{
	string whitespace = " \t\r\n";
	string::iterator start = s.begin();
	string::iterator end   = s.end();
	
	// Skip all whitespace untill you find a valid char or the input string is finished
	while (start != end && find(whitespace.begin(), whitespace.end(), *start) != whitespace.end())
		++start;
	
	if (start == end)
		return "";

	--end; // make end point to the last char of s
	while (start != end && find(whitespace.begin(), whitespace.end(), *end) != whitespace.end())
		--end;
	
	if (start == end)
		return "";

	return string(s, start-s.begin(), end-start+1);
}

void MethodAnalyzer::PopulateArgumentVec(ArgumentVec & av, std::string s)
{
	Tokenizer t(s,",");

	string arg;
	while (!(arg = t.GetNextToken()).empty())
	{
		arg = Trim(arg);

		MethodArgument ma = CreateMethodArgument(arg);

		av.push_back(ma);
	}
}

MethodArgument MethodAnalyzer::CreateMethodArgument(std::string s)
{
	MethodArgument ma;

	Tokenizer t(s," =");

	ma.type			= t.GetNextToken();
	ma.name			= t.GetNextToken();
	ma.defaultValue	= t.GetNextToken();

	return ma;
}
