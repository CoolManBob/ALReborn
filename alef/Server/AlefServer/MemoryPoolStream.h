#ifndef _MEMORY_POOL_STREAM_H_
	#define _MEMORY_POOL_STREAM_H_

#include "ApBase.h"
#include "AuIniManager.h"

class MemoryPoolStream {
private:
	AuIniManagerA	m_csIniFile;

public:
	INT32	m_lCircularBuffer;
	INT32	m_lCircularOutBuffer;

	INT32	m_lFactorTypeResult;
	INT32	m_lFactorTypeCharStatus;
	INT32	m_lFactorTypeCharType;
	INT32	m_lFactorTypeCharPoint;
	INT32	m_lFactorTypeCharPointMax;
	INT32	m_lFactorTypeDamage;
	INT32	m_lFactorTypeDefense;
	INT32	m_lFactorTypeAttack;
	INT32	m_lFactorTypeItem;
	INT32	m_lFactorTypeDirt;
	INT32	m_lFactorTypePrice;
	INT32	m_lFactorTypeOwner;
	INT32	m_lFactorTypeAgro;
	INT32	m_lAgpdFactor;

	INT32	m_lCharacter;
	INT32	m_lItem;
	INT32	m_lSkill;

	INT32	m_lGridItem;
	INT32	m_lGrid;

	INT32	m_lPvPArray;
	INT32	m_lPvPCharInfo;
	INT32	m_lPvPGuildInfo;

	INT32	m_lMapIdPos;

	INT32	m_lOptimizedPacket;

public:
	MemoryPoolStream()
	{
		m_lCircularBuffer	= 0;
		m_lCircularOutBuffer	= 0;

		m_lFactorTypeResult	= 0;
		m_lFactorTypeCharStatus	= 0;
		m_lFactorTypeCharType	= 0;
		m_lFactorTypeCharPoint	= 0;
		m_lFactorTypeCharPointMax	= 0;
		m_lFactorTypeDamage	= 0;
		m_lFactorTypeDefense	= 0;
		m_lFactorTypeAttack	= 0;
		m_lFactorTypeItem	= 0;
		m_lFactorTypeDirt	= 0;
		m_lFactorTypePrice	= 0;
		m_lFactorTypeOwner	= 0;
		m_lFactorTypeAgro	= 0;
		m_lAgpdFactor	= 0;

		m_lCharacter	= 0;
		m_lItem	= 0;
		m_lSkill	= 0;

		m_lGridItem	= 0;
		m_lGrid	= 0;

		m_lPvPArray	= 0;
		m_lPvPCharInfo	= 0;
		m_lPvPGuildInfo	= 0;

		m_lMapIdPos	= 0;

		m_lOptimizedPacket	= 0;
	}

	~MemoryPoolStream()
	{
	}

public:
	BOOL	ReadPoolCount(const CHAR *szFile);
};

#endif	//_MEMORY_POOL_STREAM_H_