// Tokenizer.h: interface for the Tokenizer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(__TOKENIZER_H__)
#define __TOKENIZER_H__

#include <string>

class Tokenizer  
{
public:
	Tokenizer(std::string input, std::string seperators);

	std::string GetNextToken();

private:
	std::string				m_input;
	std::string::iterator	m_currPos;
	std::string				m_seperators;
};

#endif // !defined(__TOKENIZER_H__)
