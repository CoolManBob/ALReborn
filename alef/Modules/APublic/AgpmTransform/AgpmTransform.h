#if !defined _AGPMTRANSFORM_H_
#define _AGPMTRANSFORM_H_

#include "ApBase.h"
#include "ApModule.h"

#include <map>
using namespace std;

//범용적인 작성을 위한 내용
//지금은 드래곤시온만 사용하지만 두루두루 사용할수있도록... 변경...

//enum AuRaceType
//{
//	AURACE_TYPE_NONE		= 0,
//	AURACE_TYPE_HUMAN,			//휴먼 
//	AURACE_TYPE_ORC,			//오크
//	AURACE_TYPE_MOONELF,		//문엘프
//	AURACE_TYPE_DRAGONSCION,	//드래곤시온
//	AURACE_TYPE_MAX
//};

//enum AuCharClassType
//{
//	AUCHARCLASS_TYPE_NONE		= 0,
//	AUCHARCLASS_TYPE_KNIGHT,
//	AUCHARCLASS_TYPE_RANGER,
//	AUCHARCLASS_TYPE_SCION,
//	AUCHARCLASS_TYPE_MAGE,
//	AUCHARCLASS_TYPE_MAX
//};

struct TransformInfo
{
	TransformInfo( INT32 nKnight, INT32 nRanger, INT32 nMage ) : m_nKnight(nKnight), m_nRanger(nRanger), m_nMage(nMage)	{	}

	INT32	m_nKnight;
	INT32	m_nRanger;
	INT32	m_nMage;
};
typedef map< INT32, TransformInfo* >	TransformInfoMap;
typedef TransformInfoMap::iterator		TransformInfoMapItr;

class AgpmTransform : public ApModule
{
public:
	AgpmTransform();
	~AgpmTransform();

	BOOL	Initialize( char* szCharacter, char* szItem, BOOL bDecryption );
	void	Release();

	INT32	GetCharacterID( INT32 nID, AuCharClassType eClassType );
	INT32	GetItemID( INT32 nID, AuCharClassType eClassType );

private:
	TransformInfoMap	m_mapCharacterInfo;
	TransformInfoMap	m_mapItemInfo;
};

#endif