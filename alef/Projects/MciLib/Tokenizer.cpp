#include "Tokenizer.h"
#include <algorithm>

using std::find;
using std::string;

Tokenizer::Tokenizer(string input, string seperators) :
	m_input(input),
	m_seperators(seperators),
	m_currPos(m_input.begin())
{	
}

string Tokenizer::GetNextToken()
{
	// Skip all seperators untill you find a valid char or the input string is finished
	while (m_currPos != m_input.end() && find(m_seperators.begin(), m_seperators.end(), *m_currPos) != m_seperators.end())
		++m_currPos;
	
	if (m_currPos == m_input.end())
		return "";

	string::iterator tokenBegin = m_currPos;

	// Skip all non-seperators until you find the 1st seperator is found or the input string is finished
	while (m_currPos != m_input.end() && find(m_seperators.begin(), m_seperators.end(), *m_currPos) == m_seperators.end())
		++m_currPos;

	string::difference_type  start = tokenBegin - m_input.begin();
	string::difference_type  end	  =	m_currPos   - m_input.begin();
	string token(m_input, start, end-start);

	return token;
}
