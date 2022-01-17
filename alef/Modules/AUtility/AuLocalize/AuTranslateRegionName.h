#ifndef __TRANSLATE_REGION_NAME_H_
#define __TRANSLATE_REGION_NAME_H_

#include <map>
#include <string>

typedef std::map<std::string, std::string> StrMap;

class CTranslate
{
public:
	CTranslate();
	CTranslate(int i);	// 서로 다른 두개의 인스턴스를 만들어야 해서 간단히
	~CTranslate();

	const char* GetName(char* key);

private:
	StrMap m_strMap;
};

const char* GetKorName(char* engName);
const char* GetEngName(char* korName);


#endif __TRANSLATE_REGION_NAME_H_