// ConCurrencyMap.h: interface for the CConCurrencyMap class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONCURRENCYMAP_H__A3B053F2_F6EE_4CA0_AE69_98C20E64B2D2__INCLUDED_)
#define AFX_CONCURRENCYMAP_H__A3B053F2_F6EE_4CA0_AE69_98C20E64B2D2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ApAdmin.h"
#include "AgpdCharacter.h"

struct ConCurrencyUser
{
	INT16			m_lNID;
	CString			m_strEncryptCode;
	CString			m_strAccountID;
	AgpdCharacter	m_stAgpdCharacter[4];
	INT32			m_lStatus;
	
	ConCurrencyUser() : m_lNID(0), m_lStatus(0)
	{
		::ZeroMemory(m_stAgpdCharacter, sizeof(m_stAgpdCharacter));
	}
};

class CConCurrencyMap : public ApAdmin
{
public:
	CConCurrencyMap();
	virtual ~CConCurrencyMap();

	BOOL Add(ConCurrencyUser *pstUser);
	ConCurrencyUser* Get(INT16 lNID);
	ConCurrencyUser* Get(LPCTSTR lpUserName);
	BOOL Remove(INT16 lNID);
};

#endif // !defined(AFX_CONCURRENCYMAP_H__A3B053F2_F6EE_4CA0_AE69_98C20E64B2D2__INCLUDED_)
