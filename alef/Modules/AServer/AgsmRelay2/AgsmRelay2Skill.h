#ifndef _AGSMRELAY2SKILL_H
#define _AGSMRELAY2SKILL_H

class AgsdRelay2SkillSave 
	:	public AgsdDBParam, 
		public ApMemory<AgsdRelay2SkillSave, 10000>
{
public:
	CHAR	m_szCharName[AGPDCHARACTER_NAME_LENGTH+1];
	UINT32	m_nRemainTime;
	INT32	m_nSkillTID;
	CHAR	m_szExpireDate[_MAX_DATETIME_LENGTH+1];
	UINT32	m_nAttribute;

	AgsdRelay2SkillSave(UINT32 ulNID, CHAR* CharName)
	{
		m_ulNID = ulNID;
		memset(m_szCharName, 0, sizeof(m_szCharName));
		strcpy(m_szCharName, CharName);

		m_nSkillTID		= 0;
		m_nRemainTime	= 0;
		memset(m_szExpireDate, 0,sizeof(m_szExpireDate));
		m_nAttribute	= 0;
	};

	virtual ~AgsdRelay2SkillSave()
	{

	};

	void Release()
	{ 
		delete this;
	};

	BOOL SetParamInsert(AuStatement* pStatement)
	{
		pStatement->SetParam( 0, m_szCharName, sizeof(m_szCharName));
		pStatement->SetParam( 1, &m_nSkillTID);
		pStatement->SetParam( 2, &m_nRemainTime);
		pStatement->SetParam( 3, m_szExpireDate, sizeof(m_szExpireDate));
		pStatement->SetParam( 4, &m_nAttribute);

		return TRUE;
	};

	BOOL SetParamDelete(AuStatement* pStatement)
	{
		pStatement->SetParam( 0, m_szCharName, sizeof(m_szCharName));
		pStatement->SetParam( 1, &m_nSkillTID);

		return TRUE;
	};
};

#endif // _AGSMRELAY2SKILL_H