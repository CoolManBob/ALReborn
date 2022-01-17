#include "AuTranslateRegionName.h"

using std::string;

// korean - English
CTranslate::CTranslate()
{
	m_strMap["Anchorville"]	= "앵커빌";
	m_strMap["Delfaras"]	= "델파라스";
	m_strMap["Tullan"]		= "툴란";
	m_strMap["Ellore"]		= "엘로르";

	m_strMap["Golundo"]		= "골룬도";
	m_strMap["Kuhn"]		= "쿤";
	m_strMap["Trilgard"]	= "트릴가드";
	m_strMap["Zylok"]		= "자일록";

	m_strMap["Norine"]		= "노린";
	m_strMap["Laflesia"]	= "라플레시아";
	m_strMap["Deribelle"]	= "데리벨";
	m_strMap["Tor Fortress"]= "토르 요새";

	m_strMap["Sienne"]		= "시엔";
	m_strMap["Sariend"]		= "사리엔드";
	m_strMap["Stull"]		= "스톨";
	m_strMap["Halien"]		= "할리엔";

	m_strMap["Hideback"]	= "하이드백 마을";
	m_strMap["Terranoa"]	= "테라노아";
}

// English - Korean
CTranslate::CTranslate(int i)
{
	m_strMap["앵커빌"]		= "Anchorville";
	m_strMap["델파라스"]	= "Delfaras";
	m_strMap["툴란"]		= "Tullan";
	m_strMap["엘로르"]		= "Ellore";

	m_strMap["골룬도"]		= "Golundo";
	m_strMap["쿤"]			= "Kuhn";
	m_strMap["트릴가드"]	= "Trilgard";
	m_strMap["자일록"]		= "Zylok";

	m_strMap["노린"]		= "Norine";
	m_strMap["라플레시아"]	= "Laflesia";
	m_strMap["데리벨"]		= "Deribelle";
	m_strMap["토르 요새"]	= "Tor Fortress";

	m_strMap["시엔"]		= "Sienne";
	m_strMap["사리엔드"]	= "Sariend";
	m_strMap["스톨"]		= "Stull";
	m_strMap["할리엔"]		= "Halien";

	m_strMap["하이드백 마을"]	= "Hideback";
	m_strMap["테라노아"]	= "Terranoa";


}

CTranslate::~CTranslate()
{
}

const char* CTranslate::GetName(char* key)
{
	StrMap::iterator iter = m_strMap.find(key);
	return (iter != m_strMap.end()) ? iter->second.c_str() : "";
}

const char* GetKorName(char* engName)
{
	static CTranslate ek;
	return ek.GetName(engName);
}

const char* GetEngName(char* korName)
{
	static CTranslate ke(1);
	return ke.GetName(korName);
}
